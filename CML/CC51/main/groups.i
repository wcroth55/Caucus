#
#---groups.i    Return list of all user groups in variable 'groups'.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 04/07/03 New script.
#--------------------------------------------------------------------------

set groups
for group size code in $dirlist(2 $caucus_path()/GROUPS/)
   #---No directories, or dot-files, please! <g>
   if $and ($equal ($(code) 0)  $not_equal ($str_sub (0 1 $(group)) .))
      set groups $(groups) $(group)
   end
end
