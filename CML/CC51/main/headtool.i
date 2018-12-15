#
#---HEADTOOL.I   Put up Caucus toolbar.
#
#  $inc(1)  pagename for help
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/09/02 Support center bkg colors, png images.
#: CR 06/29/05 Take &nbsp;'s out of conference display names.
#----------------------------------------------------------------------

"<TABLE width="100%" CELLSPACING=0 CELLPADDING=5
if $equal ($(icon_type) gif)
"   BGCOLOR="$(tool_color)"  $(tool_border)
end
">

"<TR valign=top>
if $not_empty  ($rest (2 $site_data (0 - if_$(iface)_allogo)))
   "<TD>$mac_expand($reval ($rest (2 $site_data (0 - if_$(iface)_allogo))))</TD>
end

"<TD ALIGN=LEFT>

#--- If we've been passed a page name variable, use it, otherwise
#    just pull the page name out of $(nxt) and put it in big type.
if $not_empty($(pagename))
   set pname $str_replace(&nbsp; $quote( ) $(pagename))
   "<B>$(pname)</B>
end
else
   set pname $str_replace(&nbsp; $quote( ) $rest(4 $page_get($(nxt))))
   "<H2>$(pname)</H2>
end
"</TD>
#"<TD>&nbsp;</TD>
"<TD ALIGN=right>

set menu $word (1 $site_data (0 - if_$(iface)_mbutton) menu_default.i)
include $(main)/$(menu)  $inc(1)

"</TD></TR>
"</TABLE>
#----------------------------------------------------------------------
