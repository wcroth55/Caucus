#
#---cen_myconfs.i    "My Conferences" block.
#
#   Inputs
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 09/13/05 New file.
#: CR 09/21/05 Detailed/dates layout changes.
#: CR 10/20/05 Conf order, skip interface_storage confs.
#: CR 12/23/05 Don't show cnums <= 0 !
#: CR 12/26/05 If empty arg(3), use x's.  (E.g. if called from $includecml())
#: CR 01/04/06 Skip interface_storage, including mixed-case variants.
#: CR 10/10/08 Add handling for individual item perms.
#: CR 10/28/08 Use new $priv() function for privilege levels.
#: CR 11/29/09 Simply "new" calculations if no individually-permitted items.
#: CR 02/28/11 Add $config(individualItems) and $config(disableIndividualItems)
#              options for calculating effects of individually-permissioned items.
#-----------------------------------------------------------------------

"<tr><td colspan=12><b>My Conferences:&nbsp;</b>&nbsp;
   "  <A HREF="readnew.cml?$(nch)+$(nxt)" %help(h_read)
   ">read all new discussion</A>, or 
   if $empty($arg(3))
      "  <a href="conflistedit.cml?$(nch)+$(nxt)+x+x+x+x+x+m"
   end
   else
      "  <a href="conflistedit.cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+$arg(5)+\
                                   $arg(6)+$arg(7)+m"
   end
   "><nobr>edit my conference list</a>.</nobr></td>
  
"<tr valign=bottom><TD WIDTH=15></TD>
     "<TD >Conference name:
         "&nbsp;&nbsp;&nbsp;</TD>
     "<TD WIDTH=15></TD>
     "<TD ALIGN=right># of Items with<BR>New discussion</TD>


  #---Did the user request "detailed listing" (from their personal
  #   information page)?  Else default to just one column of info per conf.
  set detail $user_data ($userid() item_detail)
  if $(detail)
     "<TD></TD>
     "<TD>&nbsp;&nbsp;&nbsp;</TD>
     "<TD ALIGN=center># of New<BR>Items</TD>
     "<TD></TD>
     "<TD>&nbsp;&nbsp;&nbsp;</TD>
     "<TD ALIGN=right># of New<BR>Responses</TD>
  end
  else
     "<TD WIDTH=45>&nbsp;</TD>
  end

  #---Did the user ask to see modification dates?
  set dates $user_data ($userid() conf_dates)
  if $(dates)
     "<td>&nbsp;&nbsp;</td>
     "<TD ALIGN=right>Last modified</TD>
  end
  else
     "<TD></TD><TD></TD>
  end

  #---For each conf in their list...
  set query  \
      SELECT a.name, a.cnum, a.access, o.alpha FROM \
         (SELECT c.name, c.cnum, g.access FROM (confs c, groups g) \
            LEFT JOIN confhidden h \
              ON (h.userid = '$userid()' AND  g.name = h.cnum) \
           WHERE  g.userid = '$userid()' AND  g.active > 0  AND g.access >= $priv(minimum) \
             AND  g.owner = 'CONF'  AND  c.cnum = g.name \
             AND  c.cnum  > 0 \
             AND  c.deleted = 0 \
             AND  h.hidden IS NULL) a \
        LEFT JOIN conforder o \
          ON (o.userid = '$userid()'  AND o.cnum = a.cnum) \
       ORDER BY ifnull(o.alpha, a.name)

# "<tr><td colspan=8>$(query)</td>

  set andIndividualItemsQuery \
      AND (    (SELECT count(*) FROM groups WHERE owner='Item' AND name=i.id) = 0 \
           OR  (SELECT access   FROM groups WHERE owner='Item' AND name=i.id \
                   AND userid='$userid()') >= $priv(minimum))

  set ismgr %mgr_in_group ($userid() conferenceChange conferenceAllPrivileges \
                           interfaceManage)
  set h $sql_query_open ($(query))
  while $sql_query_row  ($(h))

     #---Skip interface_storage confs, unless appropriate manager or organizer.
     if $or ($equal ($str_index (interface_storage $lower($(name))) 0) \
             $equal (system_announcements          $lower($(name))))
        if $and ($not($(ismgr))  $less ($(access) $priv(instructor)))
           continue
        end
     end

     set dname $replace (_ 32 $(name))
     "<TR>
     "<TD WIDTH=15></TD>

     #---Make sure user's participation record exists.
     include $(main)/confpartic.i $(cnum)

     #---See if there are any individually-permissioned items in this conf.
     set individualItem
     if $not($tablefind ($userid() $unquote($config(disableIndividualItems))))
        if $config(individualItems)
           set qIndividual SELECT g.name AS individualItem  \
                             FROM grouprules g, conf_has_items h \
                            WHERE g.owner='Item' AND g.name = h.items_id \
                              AND h.cnum = $(cnum) LIMIT 1
           eval %sql1row ($(qIndividual))
        end
     end
     
     #---Calculate # of new responses (rnew), # of items with new responses (wnew).
     set q SELECT COUNT(*) wnew, SUM(i.lastresp - s.seen) rnew \
             FROM items i, conf_has_items h, resps_seen s \
            WHERE h.cnum=$(cnum)      AND h.items_id = i.id  \
              AND h.hidden=0          AND h.retired=0 \
              AND h.deleted=0 \
              AND s.items_id = i.id   AND s.userid='$userid()' \
              AND i.lastresp > s.seen AND s.forgot=0 
     if $not_empty ($(individualItem))
        set q $(q) $(andIndividualItemsQuery)
     end
#    set q1Time $timems()
     eval %sql1row ($(q))
#    set q1Time $minus ($timems() $(q1Time))

     #---Calculate # new items (inew).
     set q SELECT COUNT(*) inew \
             FROM (items i, conf_has_items h) \
             LEFT JOIN resps_seen s  \
                    ON (s.userid='$userid()' AND s.items_id = i.id) \
            WHERE h.cnum=$(cnum) AND h.items_id = i.id AND s.items_id IS NULL \
              AND h.hidden=0     AND h.retired=0   AND h.deleted=0
     if $not_empty ($(individualItem))
        set q $(q) $(andIndividualItemsQuery)
     end
#    set q2Time $timems()
     eval %sql1row ($(q))
#    set q2Time $minus ($timems() $(q2Time))

     set wnew $plus ($(wnew) $(inew))

     #---Predefine URLs for all new material (ANEW), new items
     #   (IREF), and new responses (RREF).
     set aref
     if $not_equal ($(wnew) 0)
        set aref <A HREF="anew.cml?$(nch)+$(nxt)+\
                          $(cnum)" %help(h_anew)>$(wnew)</A>
     end
     set iref
     if $not_equal ($(inew) 0)
        set iref <A HREF="inew.cml?$(nch)+$(nxt)+\
                          $(cnum)" %help(h_inew)>$(inew)</A>
     end
     set rref
     if $not_equal ($(rnew) 0)
        set rref <A HREF="rnew.cml?$(nch)+$(nxt)+\
                          $(cnum)" %help(h_rnew)>$(rnew)</A>
     end
  
     #---Conf name (and link to home page)
     "<TD><A HREF="confhome.cml?$(nch)+$(nxt)+\
                $(cnum)+x+x+x+x+x" %help(h_cname)>$(dname)</A></TD>
     "<TD></TD>

     #---Links to simple or detailed new material.
     "<TD ALIGN=right>$(aref)</TD>
     "<TD></TD>
     if $(detail)
        "<TD></TD>
        "<TD ALIGN=right>$(iref)</TD>
        "<TD></TD>
        "<TD></TD>
        "<TD ALIGN=right>$(rref)</TD>
     end
     set modified  $site_data($(cnum) - modified)
     if $and (0$(dates) 0$(modified))
        "<TD></TD>
        "<TD ALIGN="right"><NOBR><SMALL
        "    >%datetz ($dateof (\
              %epoch_of($site_data($(cnum) - modified))))
        "$(timezonename)</SMALL></NOBR></TD>
     end

     #---Diagnostics for timing effect of individual item queries.
#    "<td align=right>$(q1Time) ms</td>
#    "<td align=right>$(q2Time) ms</td>
#    "<td align=right>[$(individualItem)]</td>

  end
  eval $sql_query_close($(h))
