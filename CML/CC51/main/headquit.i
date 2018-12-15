#
#---HEADQUIT.I
#
#   Caucus logo and toolbar header for pages that only get the QUIT button.
#
#   Input:
#     $inc(1)        page title (if any)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/09/02 Support center bkg colors, png images.
#---------------------------------------------------------------------

"<TABLE width="100%" CELLSPACING=0 CELLPADDING=0 
if $equal ($(icon_type) gif)
"  BGCOLOR="$(tool_color)"  $(tool_border)
end
">

"<TR>
#  "<TD><H1><IMG SRC="$(icon)/clearpix.gif" width=4 HEIGHT=35 ALT="">
   "<TD><H1>$replace(_ 32 $inc(1))</H1></TD>
   "<TD ALIGN=RIGHT><IMG $(src_logo)
   "  ><A HREF="goodbye.cml" %help(h_quit)><IMG
          "$(src_1quit)></A
   "></TD>
"</TABLE>
