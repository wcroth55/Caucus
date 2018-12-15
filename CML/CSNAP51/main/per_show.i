#
#---PER_SHOW.I     Display general information about a person.
#
#   Input:
#     (uid)    userid of person
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#--------------------------------------------------------------

#---Each field of info about them.
set picture $user_info ($(uid) picture)
"<P>
"<TABLE $if ($not_empty ($(picture)) WIDTH=100%)>
"<TR VALIGN=top>
"<TD>Home Page:</TD>  <TD></TD>


set url $user_info($(uid) homepage)
if $not_empty ($(url))
   set http $lower($word (1 $replace (/ 32 $(url))))
   if $not_equal ($(http) http:)
      set url http://$(url)
   end
end
"<TD>$t2url($(url))</TD>

if $not_empty ($(picture))
   "<TD ROWSPAN=7 ALIGN=right>
   "<IMG SRC="$(picture)" ALT="picture">
   "</TD>
end


"<TR>
"<TD>E-mail:</TD>  <TD></TD>
set e_mail $user_info($(uid) email)
if $and ($empty($(e_mail))  $not_empty($(guess_email)) )
   set e_mail $(uid)@$(guess_email)
end
"<TD><A HREF="mailto:$(e_mail)">$(e_mail)</A></TD>

"<TR>
"<TD>Telephone:</TD>  <TD></TD>
"<TD> $per_phone($(uid))</TD>

"<TR>
"<TD>Introduction:</TD>

set prohibited $(tag_list)
if $not_equal(x$site_data($(vi_cnum) - tag_list_user) x)
   set prohibited $site_data($(vi_cnum) - tag_list_user)
end
"<TR>
"<TD COLSPAN=3>
"<UL>
   "<FONT SIZE=-1>
      "$cleanhtml (prohibited \
           $protect($mac_expand($wrap2html($per_intro($(uid))))))
   "</FONT>
"</UL>
"</TD>

"<TR>
"<TD>Last on Caucus:</TD><TD></TD>
"<TD>$per_laston($(uid))</TD>

#--------------------------------------------------------------
