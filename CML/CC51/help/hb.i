#
#--- HB.I    Help Bottom include file
#
#  inc(3) means "don't show Back button" if 1
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#

"<P>
"<FORM NAME="helpform2">
"<TABLE CELLSPACING=0 CELLPADDING=0 WIDTH="100%">
"<TR>
"<TD>&nbsp;</TD>
"<TD ALIGN=right>
if $not (0$inc(1))
   "<INPUT TYPE="button" VALUE="Back"  onClick="history.back();">
end
" <INPUT TYPE="button" VALUE="Close" 
"   onClick="window.opener.pop_gone('help'); window.close();"></TD>
"</TABLE>
"<HR>
"</FORM>

"</BODY>
"</HTML>
