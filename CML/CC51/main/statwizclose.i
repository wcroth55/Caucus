#
#---statwizclose.i    "Close" output for stat wizard
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 07/26/03 New script.
#-------------------------------------------------------------------

if $equal ($form(restype) w)
   "</table>
   "</div>
   "</body>
   "</html>
end
else
   eval $close ($(fd))
   eval $copyfile($(base).tmp $(base).xls 644)
   eval $delfile ($(base).tmp)
   "%location (http:/~$caucus_id()/TEMP/$(pid).xls)
end
