#
#---INDV_CNC.I   Create New Category
#                Put up page asking user for new notebook category,
#                then add checked responses to it.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/13/04 Use CSS.
#-----------------------------------------------------------------

"Content-type: text/html
"
"<HTML>
"<HEAD>
"<TITLE>Create New Notebook Category</TITLE>
include $(main)/css.i 1
"</HEAD>

"<BODY $(body_bg) ONLOAD="document.newcat.category.focus();">
"<font size="+1">Create New Notebook Category</font>

"<FORM NAME="newcat" METHOD=POST 
"    ACTION="$(mainurl)/indv_cncf.cml?$arg(1)+$arg(2)+$arg(3)">
"<INPUT TYPE=HIDDEN NAME=checklist VALUE="$form(checklist)">

"<P>
"New Category:&nbsp;
"<INPUT TYPE=TEXT NAME=category SIZE=18 VALUE="">
"<P>
"<TABLE CELLSPACING=0 CELLPADDING=0>
"<TR>
"<TD VALIGN=top><INPUT TYPE=submit NAME=create VALUE="Create">&nbsp;</TD>
"<TD>new category and add checked responses to it.</TD>

"<TR><TD>&nbsp;</TD>

"<TR>
"<TD ALIGN=center><INPUT TYPE=button NAME=close  VALUE="Close"
"     onClick="window.opener.pop_gone('indv');  window.close();"></TD>
"<TD>this window</TD>
"</TABLE>
"</FORM>

"<P>
"<HR>
"</div>
"</body>
"</HTML>
#------------------------------------------------------------
