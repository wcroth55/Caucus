#
#---filesafes.i     List subset of filesafes viewable by current user
#
#   Invoked as:
#      $includecml (filesafes.i args prefix)
#
#   Arguments:
#      inc(1)  Caller's $arg(2)+$arg(3)...+$arg(8)
#      inc(2)  filesafe list prefix:
#                 if "xyz", then only list filesafes starting with "xyz..."
#                 if empty, list all viewable filesafes
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 10/03/05 C5 changes.
#-----------------------------------------------------------------------

set fs    $list_site_data (0 - FN_$lower($inc(2)))
set fsdex $gen_sort (1 $(fs))
for fsi in $(fsdex)
   set fsn $str_sub (3 1000 $word ($(fsi) $(fs)))
   if $gt_equal ($group_access ($userid() Filesafe $(fsn)) $priv(minimum))
      "<a href="$(mainurl)/filesafe.cml?$unique()+$inc(1)+$(fsn)"
      "  >$upper1 ($replace (_ 32 $(fsn)))</a><br>
   end
end
