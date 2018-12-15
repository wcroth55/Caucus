#
#---MKCC.I       Make "Caucus Center" page.
#
#   Argument:
#
#   Purpose:
#
#   Inputs:
#      target
#
#   How it works
#
#   Notes:
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR 12/16/01 18:38 use caucus center logo for interface $(iface).
#: CR 04/26/06 Use C5 version of "center blks".
#==========================================================================

eval $output ($(target)/temp)

"<HTML>
"<HEAD>
"<TITLE>$(center_name)</TITLE>
include $(dir)/popup.i
include $(dir)/css.i
"</HEAD>

"<BODY BGCOLOR="#FFFFFF">

"$(pagename)
set pagename <FONT SIZE=+2><B><NOBR>$(center_name)</NOBR></B></FONT>

"<TABLE WIDTH=100% CELLSPACING=0 CELLPADDING=5>
"<TR>
"<TD ALIGN=LEFT VALIGN=MIDDLE>

if $not_empty  ($rest (2 $site_data (0 - if_$(iface)_cclogo)))
   "<TD>$mac_expand($reval ($rest (2 $site_data (0 - if_$(iface)_cclogo))))</TD>
end

if $not (0$site_data (0 - if_$(iface)_cchide))
   "<TD ALIGN=LEFT VALIGN=MIDDLE>$(pagename)</TD>
end

"<TD>&nbsp;</TD><TD ALIGN=right>

include $(dir)/nav_tool.i  0000

"</TD></TR></TABLE>
#----------------------------------------------------------------------
 
#if $not_empty  ($rest (2 $site_data (0 - if_$(dir)_cclogo)))
#   "<TD>$mac_expand($reval ($rest (2 $site_var (if_$(dir)_cclogo))))</TD>
#end
#
#if $not (0$site_var (if_$(dir)_cchide))
#   "<TD ALIGN=LEFT VALIGN=MIDDLE>$(pagename)</TD>
##  "<TD ALIGN=LEFT >$(pagename)</TD>
#end

"<TABLE CELLSPACING=0 CELLPADDING=0 WIDTH=100% BORDER=0>
   "<TR>
   "<TD VALIGN=CENTER><IMG $(src_compass) ALIGN=LEFT>
   "<BR><FONT SIZE=-1><NOBR>
   "$(center_name)
   "</NOBR>
 
   "</FONT></TD>
   "<TD>&nbsp;</TD>
   "<TD ALIGN=right VALIGN=CENTER>
      include $(dir)/youare.i $userid()
   "</TD>
"</TABLE>

#---Interpret each block in turn.
count ib 1  $site_data (0 - if_$(iface)_blks)
   set  w   $site_data (0 - if_$(iface)_blk$(ib))
   set  w1  $str_sub  (0 1 $(w))
   set  w2  $rest     (2   $(w))

   if $tablefind ($(w1) b p a j r n 0)
   end
   elif $not_empty  ($rest (2 $site_data (0 - if_$(iface)_blk$(ib))))
      "$mac_expand($reval ($rest (2 $site_data (0 - if_$(iface)_blk$(ib)))))
   end

end

"<P>
"<OL>
   for cnum in $(cnums)
       set fcnum $mac_expand (%digit4($(cnum)))
      "<LI><A HREF="../C$(fcnum)/index.htm">\
              %displayed_conf_name ($(cnum))</a>
   end
"</OL>

#---If no center "blocks" set, use default pattern.

#---------------------------------------------

"<P>
"<HR>

"</BODY>
"</HTML>
eval $output()
eval $copyfile ($(target)/temp $(target)/C0000/index.htm 755)

#---Write master index.htm file that points to C0000/index.htm.
eval $output($(target)/temp)
"<HTML>
"<HEAD>
"<SCRIPT LANGUAGE="JavaScript">
"   location.href = "C0000/index.htm";
"</SCRIPT>
"</HEAD>

"<BODY BGCOLOR="#FFFFFF">
"<P>
"<NOSCRIPT>
"Go to <A HREF="C0000/index.htm">Caucus Center</A>.
"</NOSCRIPT>
"</BODY>
"</HTML>
eval $output()
eval $copyfile ($(target)/temp $(target)/index.htm 755)

#---Copy css file.
eval $copyfile ($caucus_path()/public_html/caucus5.css $(target)/caucus5.css)
#------------------------------------------------------------
