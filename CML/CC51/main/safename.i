#
#---safename.i   Implementation of %safename() macro.  Translates a 
#                string into all printable characters (thus making it
#                a "safe" filename).
#
#                Translates all non-printing characters (including a blank)
#                into underscores.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 03/02/05 New file.
#----------------------------------------------------------------------

set _f
for _s in $charvals($inc(1))
   if $or ($less ($(_s) 33)  $greater ($(_s) 126))
      set _s 95
   end
   set _f $(_f)$char($(_s))
end
"$(_f)
