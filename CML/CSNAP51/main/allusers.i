#
#---ALLUSERS.I   Write a list of all users registered across all conferences.
#
#   Input:
#      allusers   variable containing list of all userids.
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names

eval $output ($(target)/temp)

"<HTML>
"<HEAD>
"<TITLE>All Users</TITLE>
include $(dir)/css.i
"</HEAD>

set bgcolor BGCOLOR="#FFFFFF"
"<BODY $(bgcolor)>

set pagename <H2>All Users</H2>
include $(dir)/headtool.i $inc(1)

"<TABLE CELLSPACING=0 CELLPADDING=0 WIDTH=100% BORDER=0>
   "<TR>
   "<TD VALIGN=CENTER><IMG $(src_compass) ALIGN=LEFT>
   "<BR><FONT SIZE=-1><NOBR>
   "<A HREF="../C0000/index.htm">$(center_name)</A>
   "</NOBR>
   "$(compass_arrow)
   "<A HREF="#">All Users</A>
   "</TD>
"</TABLE>

"<TABLE>
for uid in $(allusers)
   if $not_empty ($user_info($(uid) lname))
      "<TR>
      "<TD><IMG SRC="$(img)/bullet.gif" WIDTH=10 HEIGHT=10></TD>
      "<TD><A HREF="../PEOPLE/$(uid).htm">%per_name($(uid))</A> </TD>
      "<TD>($(uid))</TD>
   end
end
"</TABLE>

"<P>
"<HR>
"</BODY>
"</HTML>

eval $output()
eval $copyfile ($(target)/temp $(target)/C0000/members.htm 755)
#--------------------------------------------------------------
