#
#---man_grpload.i   Load group into list0.
#
#   Purpose: utility "function" for man_grpedit stuff.  
#   Loads group named in variable in inc(1) onto the end of variable list0.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 04/04/03 New script.
#---------------------------------------------------------------

set fd $open ($caucus_path()/GROUPS/$($inc(1)) r)
while $readln ($(fd) line)
   if $not_equal ($str_sub (0 1 $(line)) #)
      set list0 $(list0) $word (1 $(line))
   end
end
eval $close ($(fd))
