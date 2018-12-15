#
#---peoplelist.i   List people...
#
#   Input:
#      inc(1) original page name
#      inc(2) show last in cname?
#      inc(3) name of query variable
#      inc(4) put in checkboxes?
#      inc(5) LIMIT if any
#      inc(6) default field
#      inc(7) optional fields
#      arg(9) sorting choice and direction
#   Output:
#      count  number of users found
#      $form(ibox) list of checked userids
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 02/21/06 New file.
#: CR 03/29/06 Make columns sortable by optional fields.
#: CR 04/26/06 Add special handling for 'picture' field.
#: CR 04/26/06 Check fields for whether they should be visible.
#: CR 05/14/06 Change javascript urls to onClicks.
#: CR 06/19/06 Ensure $(thiscol) isn't "x"!
#: CR 09/04/07 For lastin column, use old $per_lastin() data if no new data.
#: CR 08/02/09 Do not display (userid) if global show_id is false.
#: CR 08/24/09 Remove all uses of old $per_lastin().
#: CR 08/24/09 Interface-specific showUserid controls "(userid)" display instead of show_id.
#-------------------------------------------------------------------

set args    $(nch)+$(nxt)+$arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)

#---Get list of fields we are allowed to show this person!
set is_mgr $mgr_in_group ($userid() conferenceChange conferenceAllPrivileges \
             groupEditNonManager groupEditAll userReports systemAllPrivileges)
set qf SELECT name FROM user_iface WHERE iface='$(iface)' \
             AND (on_public > 0 $ifthen ($(is_mgr)  OR on_mgr  > 0) )
set showFields $sql_query_select($(qf))
set showFields $(showFields) lastin

#---%direction(field)
set thisCol $word(1 $replace(- 32 $arg(9)))
set ascDesc $word(2 $replace(- 32 $arg(9)))
if $or ($empty($(thisCol)) $equal (x$(thisCol) xx))
   set thisCol $inc(6)
end

if $not($tablefind (x$(ascDesc) xasc xdesc))
   set ascDesc asc
end
set revThis $if ($equal($(ascDesc) asc) desc asc)
eval $mac_define (direction \$if (\$equal(@1 \$(thisCol)) \$(revThis) asc))
eval $mac_define (showArrow \$ifthen (\$equal (@1 \$(thisCol)) \
      &nbsp;<img \$(\$if (\$equal(\$(ascDesc) asc) src_dnarrow src_uparrow))>))

"<tr valign=bottom>
"$ifthen ($inc(4) <td></td>)
"<td></td>
"<td><b><a href="$inc(1)?$(args)+lname,fname-%direction(lname,fname)"
"   >Name</a></b>%showArrow(lname,fname)</td>   <td>&nbsp;&nbsp;&nbsp;</td>

if $not ($tablefind (-userid $inc(7)))
   if $(showUserid)
      "<td><b><a href="$inc(1)?$(args)+userid-%direction(userid)"
      "   >Userid</a></b>%showArrow(userid)</td> <td>&nbsp;&nbsp;&nbsp;</td>
   end
end

if $and ($not ($tablefind (-laston $inc(7)))  $tablefind (laston $(showFields)))
   "<td><b><a href="$inc(1)?$(args)+laston-%direction(laston)"
   "   >Last on Caucus</a></b>%showArrow(laston)</td>
   "<td>&nbsp;&nbsp;&nbsp;</td>
end

if $inc(2)
   if $and ($not ($tablefind (-lastin $inc(7)))  $tablefind (lastin $(showFields)))
      "<td><b><a href="$inc(1)?$(args)+lastin-%direction(lastin)"
      "   >Last in $(cname)</a></b>%showArrow(lastin)</td>
      "<td>&nbsp;&nbsp;&nbsp;</td>
   end
end

if $and ($not ($tablefind (-email $inc(7)))  $tablefind (email $(showFields)))
   "<td><b><a href="$inc(1)?$(args)+email-%direction(email)"
   "   >E-mail</a></b>%showArrow(email)&nbsp;</td>
end

#---List headers for optional fields from inc(7).
for x in $inc(7)
   if $and ($not_equal ($(x) none)  $str_index(- $(x))  \
            $tablefind ($(x) $(showFields)))
#     "<td><b>$upper1($(x))</b>&nbsp;</td>
      "<td><b><a href="$inc(1)?$(args)+$(x)-%direction($(x))"
      "   >$upper1($(x))</a></b>%showArrow($(x))&nbsp;</td>
   end
end

set qp $($inc(3)) ORDER BY
set osep
for oBy in $replace (, 32 $(thisCol))
   set qp $(qp) $(osep) $(oBy) $(ascDesc)
   set osep ,
end
if $greater ($inc(5) 0)
   set qp $(qp) LIMIT $inc(5)
end


#"<tr><td colspan=10>$(showFields)</td>
set hp $sql_query_open ($(qp))
set count  0
set parity 1
while  $sql_query_row  ($(hp))
   set count  $plus  ($(count) 1)
   set parity $minus (1 $(parity))
   "<tr class="peoplelist$(parity)" valign=top>
   "$ifthen ($inc(4) \
      <td><INPUT TYPE=checkbox NAME="ibox" VALUE="$(userid)">&nbsp;</td>)
   "<td align=right>&nbsp;$(count).&nbsp;</td>

   "<td><a href="#"
   "   onClick="pop_up('person', '$(userid)');  return false;"
   "   >$(lname), $(fname)</a></td> <td></td>

   if $not ($tablefind (-userid $inc(7)))
      if $(showUserid)
         "<td>($(userid))</td>        <td></td>
      end
   end

   if $and ($not ($tablefind (-laston $inc(7)))  \
                  $tablefind (laston $(showFields)))
      "<td><small><nobr>%monthDayYearHour($(laston))</nobr></small></td>
      "<td></td>
   end

   if $inc(2)
      if $not ($tablefind (-lastin $inc(7)))
         if $not_empty($(lastin))
            "<td><small><nobr>%monthDayYearHour($(lastin))</nobr></small></td>
         end
         else
            "<td></td>
#           set lastin $per_lastin($(userid) $(cnum) )
#           "<td><small>%datetz ($(lastin)) $(timezonename)</small></td>
         end
         "<td></td>
      end
   end

   if $and ($not($tablefind (-email $inc(7)))  $tablefind (email $(showFields)))
      "<td>$ifthen ($not_empty($(email)) \
           <a href="mailto:$(email)">$(email)</a>)&nbsp;&nbsp;</td>
   end

   #---List values of optional fields from inc(7).
   for x in $inc(7)
      if $and ($not_equal ($(x) none)  $tablefind ($(x) $(showFields)))
         if $equal ($(x) registeredon)
            set mdy %monthDayYear($($(x)))
            "<td><small>$(mdy)
            "  $ifthen($not_empty($(mdy)) $str_sub(0 5 $word(2 $($(x)))))\
               </small>&nbsp;&nbsp;</td>
         end
         elif $and ($equal ($(x) picture) $not_empty($(picture)))
            "<td><img vspace=1 src="/~$caucus_id()/LIB/PICTURES/$(userid)/_thumb.jpg"></td>
         end
         else
            "<td>$($(x))&nbsp;&nbsp;</td>
         end
      end
   end
end
eval $sql_query_close($(hp))

#-------------------------------------------------------------------
