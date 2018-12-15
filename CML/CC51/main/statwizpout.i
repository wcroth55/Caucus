#
#---statwizpout.i   Statistics wizard, by Conference, output row.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 07/26/03 New script.
#---------------------------------------------------------------------

set sum     $plus ($(count_additem) $(count_addresp))
set percent $quotient ($mult (100 $(sum)) $(total))
set percent $round ($plus ($(percent) 0.00001) 2)

if $equal ($form(restype) w)
   set parity $minus (1 $(parity))
   "<tr class="peoplelist$(parity)"
   set peopleCount $plus ($(peopleCount) 1)
   "   ><td align=right>$(peopleCount).&nbsp;</td
   "   ><td>$user_info($(lastid) lname), $user_info($(lastid) fname)</td>
   "<td align=right>$(count_login)</td>
   "<td align=right>$(sum)</td>
   "<td align=right>$(percent)</td>
end

else
   eval $writeln ($(fd) \
           $user_info($(lastid) lname), $user_info($(lastid) fname)$char(9)\
                        $(count_login)$char(9)\
                        $(sum)$char(9)\
                        $(percent))
end
