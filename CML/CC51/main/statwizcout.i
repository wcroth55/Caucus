#
#---statwizcout.i   Statistics wizard, by Conference, output row.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 07/26/03 New script.
#-----------------------------------------------------------------

if $equal ($form(restype) w)
   set parity $minus (1 $(parity))
   "<tr class="peoplelist$(parity)"
   set peopleCount $plus ($(peopleCount) 1)
   "   ><td align=right>$(peopleCount).&nbsp;</td
   "   ><td>$user_info($(lastid) lname), $user_info($(lastid) fname)</td>
   for conf in $form(confs)
      "<td align=right>$(count$(conf))</td>
   end
   "<td align=right>$(total)</td>
end

else
   set spreadsheet $user_info($(lastid) lname), $user_info($(lastid) fname)
   for conf in $form(confs)
      set spreadsheet $(spreadsheet)$char(9)$(count$(conf))
   end
   eval $writeln ($(fd) $(spreadsheet)$char(9)$(total))
end
