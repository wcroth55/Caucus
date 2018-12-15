#
#---urlfailed.i      Exit/return with a url registration failure code.
#
#   Parameters:
#      inc(1)     error code
#      failure    target URL
#
#   Notes:
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 05/04/03 New script.
#--------------------------------------------------------------------------

if $empty ($(failure))
   "Content-type: text/plain
   "
   "$inc(1)
end
else
   "Location: $(failure)?err=$inc(1)
   "
end
eval $goodbye()
quit
#--------------------------------------------------------------------------
