#
#---SITESWITCH.I  C44 definition of site-wide "switches".
#
#   There are several CML variables which are used as "switches"
#   to control exactly how a particular Caucus interface works.
#
#  Revision history:
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  2/22/02 17:15 Complete rewrite of switch handling.
#: CR 10/13/04 Remove response divider (now done in CSS), add allow bozofilter.
#: CR 02/17/05 Default email notifier daemon on.
#: CR 03/06/06 Add thumbnail size.
#==================================================================

"<H3><A NAME="confcreate">Conference Creation</A></H3>
"<BLOCKQUOTE>

set varcount 0
"<H3>Default to join newly created conferences?</H3>
"<UL>
"When a new conference is created, the creator may choose
"  to be automatically made a member of the conference.&nbsp;
"  Use this checkbox to define the default value (whether the
"  creator should or should not be made a member).
"<p>
"Conference creators default to become members?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(cc_join) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_cc_join">
"</UL>
#set cc_join  1

#--------------------------------------------------------------
set varcount $plus($(varcount) 1)
"<p>
"<H3>Default to add new conf to Personal Conf List?</H3>
"<UL>
"When a new conference is created, the creator may choose
"  to automatically add it to their personal conference list.&nbsp;
"  Use this checkbox to define the default value (whether the
"  conference should be added or not).
"<p>
"Add newly created confs to Personal Conference List?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(cc_pcl) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_cc_pcl">
"</UL>
#set cc_pcl  1

#--------------------------------------------------------------
set varcount $plus($(varcount) 1)
"<p>
"<H3>Default to customize new conference immediately?</H3>
"<ul>
"When a new conference is created, the creator may choose
"  to immediately begin customizing it.&nbsp;
"  Use this checkbox to define the default value (immediately
"  customize or not).
"<p>
"Immediately customize newly created conferences?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(cc_customize) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_cc_customize">
"</ul>
#set cc_customize  1

#--------------------------------------------------------------
set varcount $plus($(varcount) 1)
"<p>
"<H3>Default to allow everyone in new conference?</H3>
"<ul>
"When a new conference is created, the creator may choose
"  to allow everyone to be a member.&nbsp;
"  Use this checkbox to define the default value (allow all, or not).
"<p>
"Allow everyone to join newly created conferences?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(cc_allowall) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_cc_allowall">
"</ul>
#set cc_allowall  1

#--------------------------------------------------------------
set varcount $plus($(varcount) 1)
"<p>
"<H3>Default to allow e-notification in new conferences?</H3>
"<ul>
"When a new conference is created, the creator may choose
"  to allow e-mail notification from the start.&nbsp;
"  Use this checkbox to define the default value (yes or no).
"<p>
"Allow e-mail notification?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(cc_enotify) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_cc_enotify">
"</ul>

#--------------------------------------------------------------
set varcount $plus($(varcount) 1)
"<p>
"<H3>Default to show member thumbnails in new conferences?</H3>
"<ul>
"When a new conference is created, the creator may choose
"  to show member's thumbnails, next to their responses, from the start.&nbsp;
"  Use this checkbox to define the default value (yes or no).
"<p>
"Show member's thumbnails?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(cc_thumbnail) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_cc_thumbnail">
"</ul>

"</BLOCKQUOTE>

##==============================================================
#
"<H3><A NAME="debug">Debugging</A></H3>
"<BLOCKQUOTE>

set varcount $plus($(varcount) 1)
"<p>
"<H3>Report on CML errors</H3>
"<ul>
"Should errors detected in CML pages be reported to every user?&nbsp;
"   Options are "yes", "no", and "pop".&nbsp;  Yes displays the errors
"   at the end of the page, in red; pop adds a "popup" window
"   that flags the errors (useful for system administrators).
"<p>
"Report on CML Errors?&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(cmlerr_def)" SIZE=5>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_cmlerr_def">
"</ul>
#set cmlerr_def  no

"</BLOCKQUOTE>


#==============================================================
#
"<H3><A NAME="email">E-mail Switches</A></H3>
"<BLOCKQUOTE>

set varcount $plus($(varcount) 1)
"<p>
"<H3>Default Hostname for E-mail Addresses</H3>
"<ul>
"Clicking on a user's name shows, among other things, their e-mail
"   address.&nbsp;  If no e-mail address was supplied by the user, Caucus
"   can "guess" that their address is "userid@" followed by the
"   value of guess_email.
"<p>
"Leave this field blank to turn off the guessing feature.
"<p>
"Use this hostname for "guessed" e-mail addresses:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(guess_email)" SIZE=35>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_guess_email">
"</ul>
#set guess_email  

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<p>
"<H3>E-mail Participation</H3>
"<ul>
"Users without web access may participate in a conference via e-mail.&nbsp;
"   New material from a conference may be sent to them via e-mail, and
"   their replies to that mail can be automatically integrated into
"   a conference.
"
"   <P>Set this field to be the userid that in which this package was
"   installed.&nbsp;  This enables sending new material <b>to</b> such e-mail
"   participants.&nbsp;  This displays a link on each conference's
"   customization page, which the organizer can use to add e-mail
"   addresses to "subscribe" to the conference.&nbsp;
"   <P>Leave this field blank (empty) to disable this feature.

"<p>Userid for e-mail participation:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(mail_out)" SIZE=20>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_mail_out">
"</ul>
#set mail_out  

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Allow e-mail notification of new material in all conferences</H3>
"<ul>
"If this switch is on (checked), <B>all</B> conference organizers may choose
"   to automatically send e-mail on a regular basis to the conference members,
"   informing them of new material in that conference.
"   <P>(Alternately, you may choose to allow e-mail notification
"   in specific conferences.&nbsp;  See Conference Settings.)
"<p>
"Let all organizers send e-mail notification of new material?
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(e_notify) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_e_notify">
"</ul>
#set e_notify  0

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Run the e-mail notifier daemon?</H3>
"<ul>
"Run the e-mail notifier every hour.  This daemon actually processes
"   the e-mail notification requests.&nbsp;
"   If this is switch is off (unchecked), no e-mail notifications will
"   be sent, regardless of any other settings.
"<p>
"Run the e-mail notification daemon?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($word (1 $(en_run) 1) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_en_run">
"</ul>
#set en_run  1

"</BLOCKQUOTE>

#==============================================================
#
"<H3><A NAME="display">Item and Response Display</A></H3>
"<BLOCKQUOTE>

set varcount $plus($(varcount) 1)
"<H3>Responses Per Page</H3>
"<ul>
"In order to keep page load times short, long items are broken up
"   into multiple pages of a certain # of responses each.&nbsp;  This is
"   the default value for that number.&nbsp;  It may be overridden by the user.
"<p>
"Number of responses that appear in a single page:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(default_pageresps)" SIZE=5>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_default_pageresps">
"</ul>
#set default_pageresps  25

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Response Context</H3>
"<ul>
"When a user views a particular response (such as the first new response
" in an item), a certain number of immediately previous responses are
" also loaded into the page to provide "context".&nbsp;  This is the default
" value for that number.&nbsp;  It may be overridden by the user.
"<p>
"Number of responses before a selected response:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(default_context)" SIZE=5>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_default_context">
"</ul>
#set default_context  3

#--------------------------------------------------------------
#
"<h3>Response Thumbnail Size</h3>
"<ul>
"This is the maximum size of the thumbnail image (of the
"response author) that (can) appear at the top of each response.
"It must be the (maximum) width (in pixels), followed by an "x",
"followed by the (maximum) height (in pixels).&nbsp;
"For example, <b>200x100</b> means "no larger than 200 pixels wide
"by 100 pixels high".
"<p/>
set varcount $plus($(varcount) 1)
"Thumbnail size:
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(default_thumbnail)" SIZE=8>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_default_thumbnail">
"&nbsp;&nbsp;&nbsp;&nbsp;After 
"   changing the size (and pressing Submit), <b>press Reload</b> 
"   to see resized images!
"<p>
#set default_lmargin  2\%

"</ul>

#--------------------------------------------------------------
#
"<H3>Item Display Margins</H3>
"<ul>
"This must be a simple number, such as "20", which means number of pixels;
" or a percentage, such as "8%", which means percentage of the width
" of the screen.
"<p>
set varcount $plus($(varcount) 1)
"Size of left margin when viewing items:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(default_lmargin)" SIZE=5>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_default_lmargin">
"<p>
#set default_lmargin  2\%

set varcount $plus($(varcount) 1)
"Size of right margin when viewing items:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(default_rmargin)" SIZE=5>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_default_rmargin">
#set default_rmargin  2\%
"</ul>

#--------------------------------------------------------------
#
"<H3>Alert Users about New Text Editors?</H3>
"<ul>
"Caucus 4.5 supports new text editors, in addition to the
"standard text box.&nbsp; Should we alert users who don't know about
"this when they click in a text box?
"<p>
set varcount $plus($(varcount) 1)
"Alert users to new editors?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(alert_ebox) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_alert_ebox">
"</ul>
"<p>

"<H3>Text Entry Box Parameters</H3>
"<ul>
"Default width and height (in single-width characters) of all item,
" response, etc. text entry boxes.
"<p>
set varcount $plus($(varcount) 1)
"Width of text-entry box:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(default_cols)" SIZE=5>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_default_cols">
"<p>

"<p>
set varcount $plus($(varcount) 1)
"Height of text-entry box:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(default_rows)" SIZE=5>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_default_rows">
"</ul>
#set default_cols  60
#set default_rows  8

#--------------------------------------------------------------
#
#set varcount $plus($(varcount) 1)
#"<H3>Response Divider</H3>
#"<ul>
#"Divider that appears between responses:&nbsp;
#"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(between_responses)" SIZE=5>
#"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_between_responses">
#"</ul>
##set between_responses  <HR>

set varcount $plus($(varcount) 1)
"<H3>User Filter</H3>
"<ul>
"Allow users to filter out (not read) responses by other users?
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) \
                                  $if ($word(1 $(siteBozoFilter) 1) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_siteBozoFilter">
"</ul>

"</BLOCKQUOTE>

#==============================================================
#
"<H3><A NAME="manage">Management Options</A></H3>
"<BLOCKQUOTE>

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Conference List Limit for Management</H3>
"<ul>
"Maximum number of conferences displayed
" at once in management functions.&nbsp;  If the
" total number of conferences exceeds this
" some functions revert to "one at a time"
" mode.&nbsp; 25 is a good default.
"<p>
"Maximum number of confs the management functions can display at once:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(manage_maxconfs)" SIZE=5>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_manage_maxconfs">
"</ul>
#set manage_maxconfs  25

"</BLOCKQUOTE>

#==============================================================
"<H3><A NAME="misc"><A NAME="misc">Miscellaneous Interface Controls</A></H3>
"<BLOCKQUOTE>

set varcount $plus($(varcount) 1)
"<H3>Quit URL</H3>
"<ul>
" When a user quits Caucus (clicks on the Quit button),
" they can either be shown the standard quit screen,
" or sent to a specific URL.
" <P>
" Enter the desired URL in the box, or clear it to get
" the standard quit screen.
"<p>
"Quit URL?&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(goodbye_url)" SIZE=30>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_goodbye_url">
"</ul>
#set goodbye_url

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Confirm Joining a Conference?</H3>
"<ul>
"When a user clicks on a conference for the first time,
" they can be required to confirm that they really do want
" to join this conference.&nbsp;  If this box is checked, they
" are prompted for this confirmation.&nbsp;  If cleared, clicking
" on a conference immediately and automatically joins it, and adds the
" conference to their personal conference list.
"<p>
"Confirm Joining a Conference?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(confirm_join) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_confirm_join">
"</ul>
#set confirm_join  1

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Help Window Buttons</H3>
"<ul>
"Enable browser buttons on pop-up HELP window?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(help_buttons) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_help_buttons">
"</ul>
#set help_buttons  0

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Japanese Searching</H3>
"<ul>
"For the Japanese language version of Caucus, this should be on.&nbsp;
" It "turns off" the display (in search.cml) of the question about
" searching for matches at the beginning vs. in the middle of words,
" and assumes the answer is "in the middle of words".
"<p>
"Use Japanese search settings?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(japanese_search) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_japanese_search">
"</ul>
#set japanese_search  0

"</BLOCKQUOTE>

#==============================================================
#
"<H3><A NAME="passwords">Passwords, userids, security</A></H3>
"<BLOCKQUOTE>

#
set varcount $plus($(varcount) 1)
"<H3>Password Confirmation</H3>
"<ul>
"Caucus can require confirmation of a user's password before allowing
" entry into the conferences.&nbsp;  This is particularly useful in academic
" environments, where PCs may be shared by many people and Web browsers
" may be left running -- with the previous user's id and password still
" cached in the browser!
"<p> 
"Require password confirmation on Caucus startup?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(confirm_password) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_confirm_password">
"</ul>
#set confirm_password  0

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Password Changing</H3>
"<ul>
"Are users allowed to change their own password?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(change_password) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_change_password">
"</ul>
#set change_password  1

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Userid Display</H3>
"<ul>
"Whenever a response is displayed, the author's name appears with
" it.&nbsp;  You may also force Caucus to display the author's userid
" as well, to discourage "spoofing" of responses.
"<p> 
"Display "(userid") after person's name?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(show_id) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_show_id">
"</ul>
#set show_id  0

"</BLOCKQUOTE>

#==============================================================
#
"<H3><A NAME="response">Responses: </A></H3>
"<BLOCKQUOTE>

set varcount $plus($(varcount) 1)
"<H3>Response Editing</H3>
"<ul>
" Normally users may edit (change) their own responses at any time.
" Some sites object to thus "being able to change history".
"<p> 
" A value of "1" enables response editing, "0" disables it.
"<p> 
"Let users edit their own responses?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(editable) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_editable">
"</ul>
#set editable  1

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Editing Cutoff</H3>
"<ul>
" 
" Use this variable to change the lengths of time
" that you have to choose from when limiting
" users' ability to edit their own responses.
" <P>Times are in seconds.  For example:
" <BR>   5 min  =  300
" <BR>  15 min  =  900
" <BR>   1 hr   =  3600
" <BR>   1 day  =  86400
" <BR>   1 week =  604800
" <P>Be sure to format your changes or additions just like this example,
" ending each option with a | character.
"<p>
#"Options for editing cutoff
"<TEXTAREA WRAP=virtual COLS=50 ROWS=4
"          NAME=VAR$(varcount)>$(edit_times)</TEXTAREA>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="BOX_edit_times">
"</ul>
#set edit_times  300   = up to 5 minutes |    3600  = up to 1 hour |    86400 = up to 1 day

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Response copying and moving</H3>
"<ul>
" Caucus provides the ability to copy and move responses from
" one place to another.  (Responses copied by non-organizers
" show a "copied by XXX" header.)  Use this checkbox to set
" the system-wide default for whether normal users should
" be allowed to do this.  (Organizers may override the default
" for their own conferences.)
"<p> 
" A value of "1" enables response moving & copying, "0" disables it.
"<p> 
"Let users copy and move responses?&nbsp;
"<INPUT TYPE=CHECKBOX NAME=VAR$(varcount) $if($(allow_copy) CHECKED)>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="CHK_allow_copy">
"</ul>
#set allow_copy  1

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Options for prohibited tags in items and responses</H3>
"<ul>
" This variable defines a specific set of tags, which
" organizers may choose to allow or prohibit in HTML postings
" in their conferences.  (Organizers may only chose from
" the list of tags defined here; they may not add other
" tags to the prohibited list.)
" 
" <P>Prohibited tags are not displayed
" when the posting is viewed, but they remain in
" the text.
" 
" <P>Along with the set of tags, this variable defines whether
" each tag should be allowed or prohibited by default.
" (The organizers may then choose to override that default.)
" 
" <P>Each entry consists of three parts, in the form<BR>
" <TT>tag,prohibit|allow,all|tag</TT>
" 
" <P>No spaces are allowed between commas.  The first
" element is the name of the tag, the second is a
" switch determining whether the tag is normally allowed
" or prohibited, and the third is a switch determining
" whether the text between the opening and closing tag
" is also hidden, or just the tag(s) themselves.
" 
" <P>For instance, the string "applet,prohibit,all"
" would suppress display of all APPLET tags and the
" text between them.  The string "form,prohibit,tag"
" would suppress the FORM tags, but display the text
" between them.  The string "blink,allow,tag" would
" normally allow BLINK tags... but also give conference
" organizers the ability to prohibit them in their
" conference.
" 
" <P>These restrictions apply to both users and organizers.
" Individual conference organizers can set
" conference-specific prohibitions for users, and a
" different set for organizers.
"<p> 
" elements: tag,allow/prohibit,hide_what
"<p> 
"Prohibited tag options:<br>
"<TEXTAREA WRAP=virtual COLS=50 ROWS=4
"          NAME=VAR$(varcount)>$(tag_list)</TEXTAREA>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="BOX_tag_list">
#set tag_list  html,prohibit,tag   body,prohibit,tag   head,prohibit,all   title,prohibit,all   layer,prohibit,all  form,allow,all   applet,allow,all   script,allow,all   blink,allow,tag
"</ul>

"</BLOCKQUOTE>

#==============================================================
#
"<H3><A NAME="visual">Visual Effects</A></H3>
"<BLOCKQUOTE>

"<H3>Background Color</H3>
"<ul>
" This switch appears in all CML files as:
" &lt;BODY $(body_bg)&gt;
" and is meant to provide a convenient way to encode the body
" background or color (or other options), as in either of:
" 
" <P>     BGCOLOR="#FFFFFF"
" <BR>    BACKGROUND=http://yourhost.com/~caucus/wbackg1.jpg
"<p>
set varcount $plus($(varcount) 1)
"Default body background:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$escquote($(body_bg))" SIZE=20>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_body_bg">
#set body_bg  BGCOLOR="#FFFFFF"

"<p>
" <P>Some pages have areas with a contrasting background color for
" highlighting information.  This should typically be set to a color
" close to the default background above.
"<p>
set varcount $plus($(varcount) 1)
"Default body highlight:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$escquote($(body_fg))" SIZE=20>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_body_fg">
"</ul>
#set body_fg  BGCOLOR="#DDDDDD"

#--------------------------------------------------------------
#
"<H3>Toolbar Properties</H3>
"<ul>
set varcount $plus($(varcount) 1)
"Toolbar background color:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(tool_color)" SIZE=10>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_tool_color">
#set tool_color  #FFFFFF

"<p>
set varcount $plus($(varcount) 1)
"Toolbar border width:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$(tool_border)" SIZE=10>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_tool_border">
#set tool_border  BORDER=0
"</ul>

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>Compass Bar Arrows</H3>
"<ul>
"Arrows between pages on compass bar:&nbsp;
"<INPUT TYPE=TEXT     NAME=VAR$(varcount) VALUE="$t2esc($(compass_arrow))" SIZE=20>
"<INPUT TYPE=HIDDEN   NAME=NAM$(varcount) VALUE="TXT_compass_arrow">
"</ul>

#--------------------------------------------------------------
#
set varcount $plus($(varcount) 1)
"<H3>IFRAME width &amp; height</H3>
"<ul>
"Default width of IFRAMEs:&nbsp;
"<INPUT TYPE=TEXT    NAME=VAR$(varcount) VALUE="$(default_iframe_width)" SIZE=5>
"<INPUT TYPE=HIDDEN  NAME=NAM$(varcount) VALUE="TXT_default_iframe_width">
#set default_iframe_width 85%

"<p>
set varcount $plus($(varcount) 1)
"Default height of IFRAMEs:&nbsp;
"<INPUT TYPE=TEXT   NAME=VAR$(varcount) VALUE="$(default_iframe_height)" SIZE=5>
"<INPUT TYPE=HIDDEN NAME=NAM$(varcount) VALUE="TXT_default_iframe_height">
set default_iframe_height 300px

"</ul>

"</BLOCKQUOTE>
#==============================================================
