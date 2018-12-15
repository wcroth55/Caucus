#
#---rtrim.i   Right-trim string (for %rtrim macro)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 06/01/2004 New file.
#------------------------------------------------------

set last $minus ($strlen($inc(1)) 1)
while $equal ($charval ($(last) $inc(1)) 32)
   set last $minus ($(last) 1)
end
"$str_sub(0 $plus($(last) 1) $inc(1))
