#
#---CENROW1.I    Caucus Logo, tool bar (center page only)
#
#   $inc(1)      pagename for help
#   $inc(2-7)    Description of current page title.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/09/02 Support center bkg colors, png images.
#--------------------------------------------------------------------

"<A NAME="top">
"<TABLE width="100%" CELLSPACING=0 CELLPADDING=5 
if $equal ($(icon_type) gif)
"   BGCOLOR="$(tool_color)"  $(tool_border)
end
">

"<TR valign="top">
if $not_empty  ($rest (2 $site_data (0 - if_$(iface)_cclogo)))
   "<TD>$mac_expand($reval ($rest (2 $site_data (0 - if_$(iface)_cclogo))))</TD>
end

if $not (0$site_data (0 - if_$(iface)_cchide))
   "<TD ALIGN=LEFT VALIGN=MIDDLE>$(pagename)</TD>
#  "<TD ALIGN=LEFT >$(pagename)</TD>
end

"<TD ALIGN=right>
set menu $word (1 $site_data (0 - if_$(iface)_mbutton) menu_default.i)
include $(main)/$(menu)  $inc(1)
"</TD></TR>
"</TABLE>
"</A>

#------------------------------------------------------------------------
