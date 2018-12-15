#
#---BOXSIZE.I    Display "Box size" buttons for <TEXTAREA> box.
#
#  inc(1)  1=>add padding before "box size", 0=>don't.
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 08/19/09 Clean up display/tables.
#------------------------------------------------------------

"<TABLE CELLSPACING=0 CELLPADDING=0>
"<TR VALIGN="middle">
"<TD>&nbsp;&nbsp;&nbsp;&nbsp;Box&nbsp;width:&nbsp;&nbsp;</TD>
"<TD><nobr><INPUT TYPE="image"  NAME="shrink" VALUE="1" BORDER=0 $(src_arleft)
    ">&nbsp;&nbsp;<INPUT TYPE="image"  NAME="expand" 
    "  VALUE="1" BORDER=0 $(src_arrite)></nobr></TD>
"</TABLE>
