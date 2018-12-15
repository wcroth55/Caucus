#
#---groupselector.i   Pull-down menu element to select a particular group.
#
#   - Must be inside form named by inc(2).
#   - Said form must have a method called submitMe(), which
#     must submit itself, plus do any other presubmission work.
#
# Input:
#   inc(1)                    prefix (for all variable names and form elements)
#   inc(2)                    form name
#   $form(egType)  | inc(3)   type:   CONF, MGR, USER, Caucus4, Filesafe, Quiz
#   $form(egRange) | inc(4)   range:  1st element of range if relevant, else "_"
#   $form(egName)  | inc(5)   name:   name of group (conf # for conference
#                                     groups), else "_"
#   inc(6)                    create: if 1, allow creation of new group for
#                                     current user.
#
# Output vars:
#   $inc(1)Type
#   $inc(1)Range
#   $inc(1)Name
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 05/23/05 New page.
#: CR 12/22/05 Rename Caucus4 -> System groups, allow creation of System groups
#: CR 01/06/06 Only list groups that user is a member of (or manager over)!
#: CR 01/16/06 Add quiz groups.
#: CR 01/26/06 Skip interface_storage confs unless mgr/instructor access
#: CR 01/29/06 Skip Caucus4 interface_ groups unless mgr/instructor access
#: CR 02/06/06 Add quiz groups.
#-----------------------------------------------------------------------------

eval $mac_define (mgr_for \$mgr_in_group ($userid() @@ groupEditNonManager \
                  groupEditAll systemAllPrivileges))

set e $inc(1)
set f $inc(2)

"<script type="text/javascript" language="javascript">
"   function $(e)Reset (type, range, name) {
"      if (document.$(f).$(e)Type.selectedIndex == 0)  return;
"      var deg = document.$(f);
"      if (range == 0  &&  deg.$(e)Range)  deg.$(e)Range.selectedIndex = 0;
"      if (name  == 0  &&  deg.$(e)Name)   deg.$(e)Name.selectedIndex  = 0;
"      submitMe();
"   }
"</script>

set $(e)Type  $word (1 $form($(e)Type)  $inc(3))
set $(e)Range $word (1 $form($(e)Range) $inc(4))
set $(e)Name  $word (1 $form($(e)Name)  $inc(5))
set $(e)RangeOptions
set $(e)NameOptions
set $(e)Dots
set createNew  0
set user_range 0
#"egType=$(egType), egRange=$(egRange), egName=$(egName)&nbsp;

#---Conference group
if $equal ($($(e)Type) CONF)
   set $(e)Dots ...
   set $(e)RangeOptions $(nameCascadeCONF)
   set $(e)RangePos     $tablecompare ($($(e)Range) $($(e)RangeOptions))

   if $not_equal ($($(e)Name) _)
      set tName $cl_name ($($(e)Name))
      if $equal ($($(e)Range) _)
         set $(e)RangePos  $tablecompare ($(tName) $($(e)RangeOptions))
         set $(e)Range      $word ($($(e)RangePos) $($(e)RangeOptions))
      end
   end

   if $not_equal ($($(e)Range) _)
      set $(e)NameLow  $word (      $($(e)RangePos)    $($(e)RangeOptions))
      set $(e)NameHigh $word ($plus($($(e)RangePos) 1) $($(e)RangeOptions) \
                                                       ~~~~~~~~)
      if %mgr_for (conferenceChange conferenceAllPrivileges)
         set query SELECT name, cnum FROM confs \
                    WHERE name >= '$($(e)NameLow)' AND name <'$($(e)NameHigh)' \
                      AND deleted = 0 \
                      AND cnum > 0 ORDER BY name
      end
      else
         set query \
             SELECT DISTINCT c.name, c.cnum FROM confs c, groups g \
              WHERE c.name >= '$($(e)NameLow)' AND c.name < '$($(e)NameHigh)' \
                AND c.cnum > 0 AND g.owner = 'CONF'  AND  g.name = c.cnum    \
                AND c.deleted = 0 \
                AND g.userid = '$userid()'  \
                AND (g.access >= $priv(instructor) OR  \
                    (g.access >   0 AND c.name NOT LIKE 'interface_storage%')) \
                ORDER BY c.name
      end
      set $(e)NameOptions $sql_query_select($(query))
   end
end

#---Manager group
elif $equal ($($(e)Type) MGR)
   if %mgr_for ()
      set query SELECT DISTINCT name, name FROM grouprules \
                 WHERE owner = 'MGR' ORDER BY name
   end
   else
      set query \
          SELECT DISTINCT r.name, r.name FROM grouprules r, groups g \
           WHERE r.owner = 'MGR' \
             AND g.owner = 'MGR' AND g.name = r.name \
             AND g.userid = '$userid()'  AND  g.access > 0 \
           ORDER BY r.name
   end
   set $(e)NameOptions $sql_query_select ($(query))
end

#---Old Caucus4 groups
elif $equal ($($(e)Type) Caucus4)
   if %mgr_for ()
      set query SELECT DISTINCT name, name FROM grouprules \
                 WHERE owner = 'Caucus4' ORDER BY name
   end
   else
      set query \
          SELECT DISTINCT r.name, r.name FROM grouprules r, groups g \
           WHERE r.owner = 'Caucus4' \
             AND g.owner = 'Caucus4' AND g.name = r.name \
             AND g.userid = '$userid()' \
             AND (g.access >= $priv(instructor) OR  \
                 (g.access >   0 AND r.name NOT LIKE 'interface_%')) \
           ORDER BY r.name
   end
   set $(e)NameOptions $sql_query_select ($(query))
   set createNew %mgr_in_group ($userid() groupEditNonManager groupEditAll)
end

##---Quizzes
#elif $equal ($($(e)Type) Quiz)
#   if %mgr_for ()
#      set query SELECT DISTINCT q.title, r.name FROM grouprules r, quiz q \
#                 WHERE r.owner = 'Quiz' AND r.name = q.qkey AND q.deleted=0 \
#                 ORDER BY q.title
#   end
#   else
#      set query \
#          SELECT DISTINCT q.title, r.name FROM grouprules r, groups g, quiz q \
#           WHERE r.owner = 'Quiz' \
#             AND g.owner = 'Quiz' AND g.name = r.name \
#             AND g.userid = '$userid()'  AND  g.access > 0 \
#             AND r.name = q.qkey  AND q.deleted=0 \
#           ORDER BY q.title
#   end
#   set $(e)NameOptions $sql_query_select ($(query))
#end

#---Standalone filesafes
elif $equal ($($(e)Type) Filesafe)
   if %mgr_for (filesafeManage)
      set query SELECT DISTINCT name, name FROM grouprules \
                 WHERE owner = 'Filesafe' ORDER BY name
   end
   else
      set query \
          SELECT DISTINCT r.name, r.name FROM grouprules r, groups g \
           WHERE r.owner = 'Filesafe' \
             AND g.owner = 'Filesafe' AND g.name = r.name \
             AND g.userid = '$userid()'  AND  g.access > 0 \
           ORDER BY r.name
   end
   set $(e)NameOptions $sql_query_select ($(query))
end

#---Userid group
else
   set user_range   1
   set query SELECT DISTINCT owner FROM grouprules \
              WHERE owner NOT IN ('CONF', 'MGR', 'Caucus4', 'Filesafe') \
              ORDER BY owner
   set $(e)RangeOptions $sql_query_select ($(query))
   if $not ($tablefind ($userid() $($(e)RangeOptions)))
      set $(e)RangeOptions $userid() $($(e)RangeOptions)
   end

   if $not_equal ($($(e)Range) _)
      if $or (%mgr_for(userEdit) $equal ($($(e)Range) $userid()))
         set query SELECT DISTINCT name, name FROM grouprules \
                    WHERE owner = '$($(e)Range)' ORDER BY name
      end
      else
         set query \
             SELECT DISTINCT r.name, r.name FROM grouprules r, groups g \
              WHERE r.owner = '$($(e)Range)' \
                AND g.owner = '$($(e)Range)' AND g.name = r.name \
                AND g.userid = '$userid()'  AND  g.access > 0 \
              ORDER BY r.name
      end
      set $(e)NameOptions $sql_query_select ($(query))
      set createNew $and ($inc(6)  $equal ($userid() $($(e)Range)))
   end
end

eval $mac_define (isType \$ifthen (\$equal(\$($(e)Type) @1) selected))


#---Type
"<td align=right><nobr><select name="$(e)Type" onChange="$(e)Reset(1, 0, 0);">
"    <option value="_">(choose)
"    <option value="Caucus4"  %isType(Caucus4)  >System
"    <option value="CONF"     %isType(CONF)     >Conference
"    <option value="Filesafe" %isType(Filesafe) >Filesafe
"    <option value="MGR"      %isType(MGR)      >Manager
#    <option value="Quiz"     %isType(Quiz)     >Quiz
"    <option value="USER"     %isType(USER)     >User
"    </select></nobr></td>


set tType $word (1 $($(e)Type) _)
if $not_equal ($(tType) _)
  
   #---Range
   if $tablefind ($($(e)Type) MGR Caucus4 Filesafe Quiz)
      "<td><div style="display: none" id="group1"
      "   ><select name="$(e)Range">
      "    <option>_
      "    <option selected>0
      "    </select></div></td>
      set tRange 1
   end
   else
      "<td align=right><nobr>&nbsp;&nbsp;<select name="$(e)Range"
      "                          onChange="$(e)Reset(1, 1, 0);">
           "<option value="_">(choose)

         if $(user_range)
           for x in $($(e)RangeOptions)
              "<option value="$(x)"
              "   $ifthen ($equal ($($(e)Range) $(x)) selected)>$(x)$($(e)Dots)
           end
         end
         else
           set xprev
           for x in $($(e)RangeOptions)
              if $not_empty ($(xprev))
                 "<option value="$(xprev)"
                 "   $ifthen ($equal ($($(e)Range) $(xprev)) selected)
                 set opt0 $(xprev)
                 if $equal ($(opt0) !)
                    set opt0 ...
                 end
                 set opt1 $(x)
                 if $equal ($(opt1) ~~~~~)
                    set opt1 ...
                 end
                 "  >$upper1($lower($unquote($(opt0)))) -
                 "   $upper1($lower($unquote($(opt1))))
              end
              set xprev $(x)
           end
         end


      "    </select></nobr></td>
      set tRange $word (1 $($(e)Range) _)
   end

   if $not_equal ($(tRange) _)

      #---Names
      if $and ($(createNew) $equal ($($(e)Name) 0))
         "<td><nobr>&nbsp;&nbsp;<input type="text" size=20 name="$(e)Name"></td>
      end
      else
         "<td><nobr>&nbsp;&nbsp;<select name="$(e)Name"
         "                          onChange="$(e)Reset(1, 1, 1);">
           "   <option value="_">(choose)
           if $(createNew)
              "<option value="0">(create new)
           end
           for x cn in $($(e)NameOptions)
           "   <option value="$(cn)" \
                   $ifthen ($equal ($($(e)Name) $(cn)) selected) \
                            >$replace(_ 32 $unquote($(x)))
           end
      "    </select></nobr></td>
      end
   end
   else
      "<td>&nbsp;&nbsp;</td>
      if $equal ($($(e)Type) CONF)
         "<td>(Narrow the choice to a range of conference names)</td>
      end
      else
         "<td>(Which user's groups do you want to see?)</td>
      end
   end
end
else
   "<td>&nbsp;&nbsp;</td>
   "<td>(Choose the type of group)</td>
end

#-------------------------------------------------------------
