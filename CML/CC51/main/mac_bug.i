#
#---MAC_BUG.I    Detect and warn about Macintosh netscape 2 & 3 bug
#   re long <SELECT> lists.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#------------------------------------------------------------------

if $and ($(is_macintosh) $not ($(is_netscape4)) \
         $or ($(is_netscape2) $(is_netscape3)))
   "<P>
   "<BLINK><B>Warning:</B></BLINK> Due to a bug in the Macintosh Netscape 2 
   "and 3, you may not be able to select the desired conference.&nbsp;
   "If you encounter this problem, try again from a non-macintosh
   "machine, or use netscape 4.
   "<P>
end
