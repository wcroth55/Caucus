#
#---THREAD.I    Display part of thread to this page.
#
#   This is a diagnostic tool, only used when debugging how
#   we get to (got to) a particular page.
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#--------------------------------------------------------------------

"<TABLE>
set prev $arg(2)
count index 1 6
   "<TR>
   "<TD>$(prev)</TD>
#  "<TD>$page_return ($(prev) #)</TD>
   "<TD>$word (1 $page_get($(prev)))</TD>
   "<TD>$rest (2 $page_get($(prev)))</TD>

   set prev $word (2 $page_get ($(prev)))
   set prev $word (1 $replace (+ 32 $(prev)))
end
"</TABLE>
#--------------------------------------------------------------------
