#
#---PERSON.I   Write a page for a person, userid inc(1).
#
#   inc(1)   userid
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR 01/06/07 Replace old $per_name().
#-------------------------------------------------------------------

set per_name $user_info ($inc(1) fname) $user_info ($inc(1) lname)

if $empty ($(per_name))
   return
end

"Processing $(per_name)
eval $output ($(target)/temp)

"<HTML>
"<HEAD>
"<TITLE>$(per_name)</TITLE>
include $(dir)/popup.i
include $(dir)/css.i
"</HEAD>

"<BODY BGCOLOR="#FFFFFF">

set pagename <H2>$(per_name) ($inc(1))<H2>
include $(dir)/headtool.i 0000

"<TABLE CELLSPACING=0 CELLPADDING=0 WIDTH=100% BORDER=0>
   "<TR>
   "<TD VALIGN=CENTER><IMG $(src_compass) ALIGN=LEFT>
   "<BR><FONT SIZE=-1><NOBR>
   "<A HREF="../C0000/index.htm">$(center_name)</A>
   "</NOBR>
   
   "$(compass_arrow)
   "<A HREF="#">$(per_name)</A>
   "</TD>
"</TABLE>
   

#---Each field of info about them.
set picture $user_info ($inc(1) picture)
if $not_empty ($(picture))
   eval $copyfile($caucus_lib()/PICTURES/$(picture) $(target)/LIB/$(picture) 755)
end
"<P>
"<TABLE $if ($not_empty ($(picture)) WIDTH=100%) >
"<TR VALIGN=top>
"<TD>Home Page:</TD>  <TD></TD>


set url $user_info($inc(1) homepage)
if $not_empty ($(url))
   set http $lower($word (1 $replace (/ 32 $(url))))
   if $not_equal ($(http) http:)
      set url http://$(url)
   end
end
"<TD>$mac_expand($t2url($(url)))</TD>

if $not_empty ($(picture))
   "<TD ROWSPAN=7 ALIGN=right>
   "<IMG SRC="../LIB/$(picture)" ALT="picture">
   "</TD>
end


"<TR>
"<TD>E-mail:</TD>  <TD></TD>
set e_mail $user_info($inc(1) email)
if $and ($empty($(e_mail))  $not_empty($(guess_email)) )
   set e_mail $inc(1)@$(guess_email)
end
"<TD><A HREF="mailto:$(e_mail)">$(e_mail)</A></TD>

"<TR>
"<TD>Telephone:</TD>  <TD></TD>
"<TD> $per_phone($inc(1))</TD>

"<TR>
"<TD>Introduction:</TD>

set prohibited $(tag_list)
#if $not_equal(x$site_data($(vi_cnum) tag_list_user) x)
#   set prohibited $site_data($(vi_cnum) tag_list_user)
#end
"<TR>
"<TD COLSPAN=3>
"<UL>
   "<FONT SIZE=-1>
      "$cleanhtml (prohibited \
           $protect($mac_expand($wrap2html($per_intro($inc(1))))))
   "</FONT>
"</UL>
"</TD>

"<TR VALIGN=top>
"<TD>Last on Caucus:</TD><TD></TD>
"<TD>$word (1 $per_laston($inc(1))) $word (2 $per_laston($inc(1)))</TD>
"</TABLE>

"<P>
"<HR>
"</BODY>
"</HTML>

eval $output()
eval $copyfile ($(target)/temp $(target)/PEOPLE/$inc(1).htm 755)
#--------------------------------------------------------------
