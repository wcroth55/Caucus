#
#---NOONE.I   Write a page for a non-existent person
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names

eval $output ($(target)/temp)

"<HTML>
"<HEAD>
"<TITLE>Person Not Registered</TITLE>
include $(dir)/popup.i
include $(dir)/css.i
"</HEAD>

"<BODY BGCOLOR="#FFFFFF">

set pagename <H2><NOBR>Person Not Registered</NOBR><H2>
include $(dir)/headtool.i 0000

"<TABLE CELLSPACING=0 CELLPADDING=0 WIDTH=100% BORDER=0>
   "<TR>
   "<TD VALIGN=CENTER><IMG $(src_compass) ALIGN=LEFT>
   "<BR><FONT SIZE=-1><NOBR>
   "<A HREF="../C0000/index.htm">$(center_name)</A>
   "</NOBR>
   
   "$(compass_arrow)
   "<A HREF="#">Not Registered</A>
   "</TD>
"</TABLE>
   
"<P>
"<B>This person is not registered with Caucus.</B>


"<P>
"<HR>
"</BODY>
"</HTML>

eval $output()
eval $copyfile ($(target)/temp $(target)/PEOPLE/_null.htm 755)
#--------------------------------------------------------------
