#
#---cen_other.i    "Other conferences" block.
#
#   For conferences that a user has "optional" access to.
#
#   Inputs
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 09/13/05 New file.
#: CR 09/21/05 Vertically align each row.
#: CR 01/04/06 Skip interface_storage, including mixed-case variants.
#-----------------------------------------------------------------------


  set header <tr><td>&nbsp;</td>\
             <tr><td colspan=8><b>Other Conferences that you may join:&nbsp;</b>&nbsp;</td>

  set ismgr %mgr_in_group ($userid() conferenceChange conferenceAllPrivileges \
                           interfaceManage)

  #---For each conf in their list...
  set query  \
      SELECT c.name, c.cnum, h.hidden FROM (confs c, groups g) \
        LEFT JOIN confhidden h \
          ON (h.userid = '$userid()'  AND  g.name = h.cnum) \
       WHERE  g.userid = '$userid()'  AND  g.active > 0  \
         AND  g.access > 0  &&  g.access < $priv(minimum) \
         AND  c.deleted = 0 \
         AND  g.owner = 'CONF'  AND  c.cnum = g.name  AND c.cnum > 0 \
       ORDER BY c.name
# "<tr><td colspan=8>$(query)</td>
# "<tr><td colspan=8>h=$(h)</td>


  set h $sql_query_open ($(query))
  while $sql_query_row  ($(h))
     if 0$(hidden)
        continue
     end

     #---Skip interface_storage confs, unless appropriate manager or organizer.
     if $or ($equal ($str_index (interface_storage $lower($(name))) 0) \
             $equal (system_announcements          $lower($(name))))
        if $and ($not($(ismgr))  $less ($(access) $priv(instructor)))
           continue
        end
     end

     "$(header)
     set header

     set dname $replace (_ 32 $(name))
  
     "<TR valign=top>
     "<TD WIDTH=15>&nbsp;</TD>
  
     #---Conf name (and link to join page)
     "<TD><A HREF="confhome.cml?$(nch)+$(nxt)+\
                $(cnum)+x+x+x+x+x" %help(h_cname)>$(dname)</A></TD>
     "<td width=15>&nbsp;</td>
     "<td>$site_data ($(cnum) - description)</td>
  end
  eval $sql_query_close($(h))
#-----------------------------------------------------------------------
