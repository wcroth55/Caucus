#
#---MEMBERS.I   Write a list of all members of a conf.
#
#   inc(1)  4-digit conf number
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR  7/11/04 include popup.i
#----------------------------------------------------------------------

eval $output ($(target)/temp)

"<HTML>
"<HEAD>
"<TITLE>$(cname) Membership List</TITLE>
include $(dir)/popup.i
include $(dir)/css.i
"</HEAD>

set bgcolor $word (1 $site_data ($inc(1) - conf_colors) BGCOLOR="#FFFFFF")
"<BODY $(bgcolor)>

set pagename <H2>$(cname) Membership List</H2>
include $(dir)/headtool.i $inc(1)

"<TABLE CELLSPACING=0 CELLPADDING=0 WIDTH=100% BORDER=0>
   "<TR>
   "<TD VALIGN=CENTER><IMG $(src_compass) ALIGN=LEFT>
   "<BR><FONT SIZE=-1><NOBR>
   "<A HREF="../C0000/index.htm">$(center_name)</A>
   "</NOBR>
   
   
   #---Below a conf home page, make conf home page clickable.
   "$(compass_arrow)
   "<A HREF="../C$(fcnum)/index.htm">$(cname)</A>
   
   #---At an item or deeper, put up item title.
   "$(compass_arrow)
   "<A HREF="#">Members</A>
   "</TD>
"</TABLE>

"<TABLE>
set last xxx.xxx
for uid in $peo_members($inc(1))
   if $and ($not_equal($(last) $(uid)) $not_empty ($user_info($(uid) lname)))
      "<TR>
      "<TD><IMG SRC="$(img)/bullet.gif" WIDTH=10 HEIGHT=10></TD>
      "<TD><A HREF="../PEOPLE/$(uid).htm">%per_name($(uid))</A> </TD>
      "<TD>($(uid))</TD>
   end
   set last $(uid)
end
"</TABLE>

"<P>
"<HR>
"</BODY>
"</HTML>

eval $output()
eval $copyfile ($(target)/temp $(target)/C$inc(1)/members.htm 755)
#-----------------------------------------------------------------
