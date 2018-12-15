#
#---statwizwout.i   Statistics wizard, by week, output row.
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
   "   ><td>$user_info($(lastid) lname), $user_info($(lastid) fname)&nbsp;</td>
   for day in 2 3 4 5 6 7 1
      "<td align=right>$(count$(day))</td>
   end
end

else
   set spreadsheet $user_info($(lastid) lname), $user_info($(lastid) fname)
   for day in 2 3 4 5 6 7 1
      set spreadsheet $(spreadsheet)$char(9)$(count$(day))
   end
   eval $writeln ($(fd) $(spreadsheet))
end
