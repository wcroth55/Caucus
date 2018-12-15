#
#---HEADTOOL.I   Put up Caucus toolbar.
#
#  $inc(1)  4-digit conference number (or 0000 for none).
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR 12/16/01 18:38 use local logo
#-------------------------------------------------

set save_m_cnum $(m_cnum)

"<TABLE WIDTH=100% CELLSPACING=0 CELLPADDING=5>
"<TR>

if $not_empty  ($rest (2 $site_data (0 - if_$(iface)_allogo)))
   "<TD>$mac_expand($reval ($rest (2 $site_data (0 - if_$(iface)_allogo))))</TD>
end

"<TD ALIGN=LEFT VALIGN=MIDDLE>

#--- If we've been passed a page name variable, use it, otherwise
#    just pull the page name out of $(nxt) and put it in big type.

"$(pagename)
"</TD><TD>&nbsp;</TD><TD ALIGN=right>

include $(dir)/nav_tool.i  $inc(1)

"</TD></TR></TABLE>

set m_cnum $(save_m_cnum)
#----------------------------------------------------------------------
