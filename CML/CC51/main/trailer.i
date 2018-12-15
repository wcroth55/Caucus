#
#---TRAILER.I     Caucus Logo, tool bar, and navigation bar.
# 
#   $inc(1)      Pagename for help
#   $inc(2-6)    Description of current page title.
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#--------------------------------------------------------------------

#"<FORM METHOD=POST ACTION="addrespf.cml?$(nch)+$(nxt)+\
#       $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)">


include $(main)/errors.i

#"<HR>
"&nbsp;<br>
#&nbsp;<br>
#&nbsp;<br>
#&nbsp;<br>

include $(main)/location.i $inc(2) $inc(3) $inc(4) $inc(5) $inc(6) $inc(7)

include $(main)/headtool.i $inc(1)

#"</FORM>

#------------------------------------------------------------------------
