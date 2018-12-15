#
#---SWITCH2.I    Define Caucus "switches" for this interface.
#
#   There are several CML variables which are used as "switches"
#   to control exactly how a particular Caucus interface works.
#
#   To change these switches, edit their definitions in this file.
#
#   The default values for these switches are contained in the
#   file defaults.i, located one level up in the directory structure.
#   DO NOT CHANGE THAT FILE!  The values in this file override the
#   default values.
#
#   As you install new versions of Caucus, you may find new switches in
#   the defaults.i file; if you wish to change them, copy the definitions
#   to switch.i and change them here.
#
#  Revision history:
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  1/31/96 12:52 1.0     release version
#: CR  6/16/96 23:29 3.0/i02 release version
#: CR 10/21/96 15:23 3.1/i04 release version
#: CR  2/14/97 18:52 3.1/final release version
#: CR  4/20/00 13:14 Add default_iframe_width, height.
#: CR 12/17/04 Add base to prohibited tag list.
#==============================================================
#
# INSTRUCTIONS for adding additional sections to this file
#
# This file is formatted so that it can be edited from the web interface.
# Any changes or additions must preserve the basic format shown here.
#
#
#    #--- VARIABLE_NAME(S)   Title for section
#    #
#    ##   Comments preceded by ## are displayed in the web interface.
#    ##   Include any HTML you need to make it display correctly.
#    #
#    #    Comments preceded by # are ignored by the web interface.
#    #
#    #    Before each variable, you need to indicate its type and give
#    #    a label for it. Type statements begin with #TYPE.  Allowable
#    #    types are TEXT, for single-line text values, CHECKBOX for
#    #    0/1 values, and TEXTAREA, for multiline text values.
#    #    For example:
#    #
#    #TYPE TEXT Use this hostname for "guessed" e-mail addresses
#    set guess_email
#
#
#==============================================================
#
#--- x <A NAME="confcreate">Conference Creation</A> <BLOCKQUOTE>
#
#--- CC_JOIN             Default to join newly created conferences?
#
##  When a new conference is created, the creator may choose
##  to be automatically made a member of the conference.
##  Use this checkbox to define the default value (whether the
##  creator should or should not be made a member).
#
#TYPE CHECKBOX Conference creators default to become members?
set cc_join  1

#--------------------------------------------------------------
#
#--- CC_PCL             Default to add new conf to Pers Conf List?
#
##  When a new conference is created, the creator may choose
##  to automatically add it to their personal conference list.
##  Use this checkbox to define the default value (whether the
##  conference should be added or not).
#
#TYPE CHECKBOX Add newly created confs to Personal Conference List?
set cc_pcl  1

#--------------------------------------------------------------
#
#--- CC_CUSTOMIZE       Default to customize new conference immediately?
#
##  When a new conference is created, the creator may choose
##  to immediately begin customizing it.
##  Use this checkbox to define the default value (immediately
##  customize or not).
#
#TYPE CHECKBOX Add newly created confs to Personal Conference List?
set cc_customize  1

#--------------------------------------------------------------
#
#--- CC_ALLOWALL       Default to allow everyone in new conference?
#
##  When a new conference is created, the creator may choose
##  to allow everyone to be a member.
##  Use this checkbox to define the default value (allow all, or not).
#
#TYPE CHECKBOX Allow everyone to join newly created conferences?
set cc_allowall  1

#--------------------------------------------------------------
#
#--- CC_ENOTIFY       Default to allow e-notification in new conferences?
#
##  When a new conference is created, the creator may choose
##  to allow e-mail notification from the start.
##  Use this checkbox to define the default value (yes or no).
#
#TYPE CHECKBOX Allow e-mail notification?
set cc_enotify  0

## </BLOCKQUOTE>

#==============================================================
#
#--- x <A NAME="debug">Debugging</A> <BLOCKQUOTE>
#
#--- CMLERR      Report on CML errors
#
##   Should errors detected in CML pages be reported to every user?
##   Options are "yes", "no", and "pop".  Yes displays the errors
##   at the end of the page, in red; pop adds a "popup" window
##   that flags the errors (useful for system administrators).

#TYPE TEXT Report on CML Errors?
set cmlerr_def  no

#--------------------------------------------------------------
#--- DEBUG      Debug Logs
#
##   Should a problem occur, the Caucus technical support representative
##   may request that you "turn debugging on".  To do so, change "$debug(0)"
##   to "$debug(1)".

#TYPE TEXT Log debugging information?
eval $debug(0)

## </BLOCKQUOTE>

#==============================================================
#
#--- x <A NAME="email">E-Mail Switches</A> <BLOCKQUOTE>
#
#--- GUESS_EMAIL   Default Hostname for E-mail Addresses
#
##   Clicking on a user's name shows, among other things, their e-mail
##   address.  If no e-mail address was supplied by the user, Caucus
##   can "guess" that their address is "userid@" followed by the
##   value of guess_email.<P>
##
##  Leave this field blank to turn off the guessing feature.<P>
#
#   Leave guess_email blank to turn off the guessing feature.
#
#TYPE TEXT Use this hostname for "guessed" e-mail addresses
set guess_email  

#--------------------------------------------------------------
#
#--- Mail_OUT     E-mail Participation
#
##   Users without web access may participate in a conference via e-mail.
##   New material from a conference may be sent to them via e-mail, and
##   their replies to that mail can be automatically integrated into
##   a conference.
#
##   <P>NOTE!!
##  <BR> To enable e-mail participation, a special package of e-mail CML
##   scripts must be installed in a userid dedicated to this purpose.
##
##   <P>Set mail_out to be the userid that in which this package was
##   installed.  This enables sending new material *to* such e-mail
##   participants.  This displays a link on each conference's
##   customization page, which the organizer can use to add e-mail
##   addresses to "subscribe" to the conference.
#
##   <P>Leave mail_out blank (empty) to disable this feature.
#

#TYPE TEXT Userid for e-mail participation
set mail_out  

#--------------------------------------------------------------
#
#--- E_Notify     Allow e-mail notification of new material in all conferences
#
##   If this switch is on (checked), <B>all</B> conference organizers may choose
##   to automatically send e-mail on a regular basis to the conference members,
##   informing them of new material in that conference.
##   <P>(Alternately, you may choose to allow e-mail notification
##   in specific conferences.  See Conference Settings.)
#

#TYPE CHECKBOX Let all organizers send e-mail notification of new material?
set e_notify  0

#--------------------------------------------------------------
#
#--- EN_RUN     Run the e-mail notifier daemon?
#
##   Run the e-mail notifier every hour.  This daemon actually processes
##   the e-mail notification requests.
##   <P>
##   If this is switch is off (unchecked), no e-mail notifications will
##   be sent, regardless of any other settings.
#

#TYPE CHECKBOX Run the e-mail notification daemon?
set en_run  0

##  </BLOCKQUOTE>
#==============================================================
#
#--- x <A NAME="display">Item and Response Display</A> <BLOCKQUOTE>
#
#--- DEFAULT_PAGERESPS   Responses Per Page
#
##   In order to keep page load times short, long items are broken up
##   into multiple pages of a certain # of responses each.  This is
##   the default value for that number.  It may be overridden by the user.

#TYPE TEXT Number of responses that appear in a single page
set default_pageresps  25

#--------------------------------------------------------------
#
#--- DEFAULT_CONTEXT     Response Context
#
##   When a user views a particular response (such as the first new response
##   in an item), a certain number of immediately previous responses are
##   also loaded into the page to provide "context".  This is the default
##   value for that number.  It may be overridden by the user.

#TYPE TEXT Number of responses before a selected response
set default_context  3

#--------------------------------------------------------------
#
#--- DEFAULT_LMARGIN,RMARGIN     Item Display Margins
#
##   This must be a simple number, such as "20", which means number of pixels;
##   or a percentage, such as "8%", which means percentage of the width
##   of the screen.

#TYPE TEXT Size of left margin when viewing items
set default_lmargin  2\%

#TYPE TEXT Size of right margin when viewing items
set default_rmargin  2\%

#--- DEFAULT_COLS        Text Entry Box Parameters
#
##   Default width and height (in single-width characters) of all item,
## response, etc. text entry boxes.

#TYPE TEXT Width of text-entry box
set default_cols  60

#TYPE TEXT Height of text-entry box
set default_rows  8

#--------------------------------------------------------------
#
#--- BETWEEN_RESPONSES       Response Divider

#TYPE TEXT Divider that appears between responses
set between_responses  <HR>

## </BLOCKQUOTE>
#==============================================================
#
#--- x <A NAME="manage">Management Options</A> <BLOCKQUOTE>
#
#--- USER_LIMIT_TRIGGER    User Limit
#
#TYPE TEXT Warn Administrator when this many user slots left
set user_limit_trigger  50

#--------------------------------------------------------------
#
#--- MANAGE_MAXCONFS  Conference List Limit for Management
# 
##                     Maximum number of conferences displayed
##                     at once in management functions.  If the
##                     total number of conferences exceeds this
##                     some functions revert to "one at a time"
##                     mode. 25 is a good default.
#
#TYPE TEXT Maximum number of confs the management functions can display at once
set manage_maxconfs  25

## </BLOCKQUOTE>

#==============================================================
#--- x <A NAME="misc">Miscellaneous Interface Controls</A> <BLOCKQUOTE>
#
#--- GOODBYE_URL    Quit URL
#
##   When a user quits Caucus (clicks on the Quit button),
##   they can either be shown the standard quit screen,
##   or sent to a specific URL.
##   <P>
##   Enter the desired URL in the box, or clear it to get
##   the standard quit screen.

#TYPE TEXT Quit URL?
set goodbye_url

#--------------------------------------------------------------
#
#--- CONFIRM_JOIN    Confirm Joining a Conference?
#
##   When a user clicks on a conference for the first time,
##   they can be required to confirm that they really do want
##   to join this conference.  If this box is checked, they
##   are prompted for this confirmation.  If cleared, clicking
##   on a conference immediately and automatically joins it, and adds the
##   conference to their personal conference list.

#TYPE CHECKBOX Confirm Joining a Conference?
set confirm_join  1

#--------------------------------------------------------------
#
#--- HELP_BUTTONS   Help Window Buttons
#
#TYPE CHECKBOX Enable browser buttons on pop-up HELP window?
set help_buttons  0

#--------------------------------------------------------------
#
#--- JAPANESE_SEARCH   Japanese Searching
#
##   For the Japanese language version of Caucus, this should be on.
##   It "turns off" the display (in search.cml) of the question about
##   searching for matches at the beginning vs. in the middle of words,
##   and assumes the answer is "in the middle of words".

#TYPE CHECKBOX Use Japanese search settings?
set japanese_search  0

## </BLOCKQUOTE>

#==============================================================
#
#--- x <A NAME="passwords">Passwords, userids, security</A> <BLOCKQUOTE>
#
#--- CONFIRM_PASSWORD     Password Confirmation
#
##   Caucus can require confirmation of a user's password before allowing
##   entry into the conferences.  This is particularly useful in academic
##   environments, where PCs may be shared by many people and Web browsers
##   may be left running -- with the previous user's id and password still
##   cached in the browser!
#
#   A value of "1" enables password confirmation, "0" disables it.

#TYPE CHECKBOX Require password confirmation on Caucus startup?
set confirm_password  0

#--------------------------------------------------------------
#
#--- CHANGE_PASSWORD     Password Changing
#
#   A value of "1" enables password changing, "0" disables it.

#TYPE CHECKBOX Are users allowed to change their own password?
set change_password  1

#--------------------------------------------------------------
#
#--- SHOW_ID       Userid Display
#
##   Whenever a response is displayed, the author's name appears with
##   it.  You may also force Caucus to display the author's userid
##   as well, to discourage "spoofing" of responses.
#
#   A value of "1" enables userid display, "0" disables it.

#TYPE CHECKBOX Display "(userid") after person's name?
set show_id  0

## </BLOCKQUOTE>

#==============================================================
#
#--- x <A NAME="response">Responses: </A> <BLOCKQUOTE>
#
#--- EDITABLE          Response Editing
#
##   Normally users may edit (change) their own responses at any time.
##   Some sites object to thus "being able to change history".
#
#   A value of "1" enables response editing, "0" disables it.

#TYPE CHECKBOX Let users edit their own responses?
set editable  1

#--------------------------------------------------------------
#
#--- EDIT_TIMES   Editing Cutoff
#
##                Use this variable to change the lengths of time
##                that you have to choose from when limiting
##                users' ability to edit their own responses.
#
##                <P>Times are in seconds.  For example:
##                 <BR>   5 min  =  300
##                 <BR>  15 min  =  900
##                 <BR>   1 hr   =  3600
##                 <BR>   1 day  =  86400
##                 <BR>   1 week =  604800
##
## <P>Be sure to format your changes or additions just like this example,
## ending each option with a | character.
##

#TYPE TEXTAREA Options for editing cutoff
set edit_times  300   = up to 5 minutes |    3600  = up to 1 hour |    86400 = up to 1 day






#--------------------------------------------------------------
#
#--- ALLOW_COPY          Response copying and moving
#
##   Caucus provides the ability to copy and move responses from
##   one place to another.  (Responses copied by non-organizers
##   show a "copied by XXX" header.)  Use this checkbox to set
##   the system-wide default for whether normal users should
##   be allowed to do this.  (Organizers may override the default
##   for their own conferences.)
#
#   A value of "1" enables response moving & copying, "0" disables it.

#TYPE CHECKBOX Let users copy and move responses?
set allow_copy  1

#--------------------------------------------------------------
#
#--- TAG_LIST   Options for prohibited tags in items and responses
#
##   This variable defines a specific set of tags, which
##   organizers may choose to allow or prohibit in HTML postings
##   in their conferences.  (Organizers may only chose from
##   the list of tags defined here; they may not add other
##   tags to the prohibited list.)
##
##   <P>Prohibited tags are not displayed
##   when the posting is viewed, but they remain in
##   the text.
##
##   <P>Along with the set of tags, this variable defines whether
##   each tag should be allowed or prohibited by default.
##   (The organizers may then choose to override that default.)
##
##   <P>Each entry consists of three parts, in the form<BR>
##   <TT>tag,prohibit|allow,all|tag</TT>
##
##   <P>No spaces are allowed between commas.  The first
##   element is the name of the tag, the second is a
##   switch determining whether the tag is normally allowed
##   or prohibited, and the third is a switch determining
##   whether the text between the opening and closing tag
##   is also hidden, or just the tag(s) themselves.
##
##   <P>For instance, the string "applet,prohibit,all"
##   would suppress display of all APPLET tags and the
##   text between them.  The string "form,prohibit,tag"
##   would suppress the FORM tags, but display the text
##   between them.  The string "blink,allow,tag" would
##   normally allow BLINK tags... but also give conference
##   organizers the ability to prohibit them in their
##   conference.
##
##   <P>These restrictions apply to both users and organizers.
##   Individual conference organizers can set
##   conference-specific prohibitions for users, and a
##   different set for organizers.
##
#  elements: tag,allow/prohibit,hide_what
#

#TYPE TEXTAREA Prohibited tag options
set tag_list  html,prohibit,tag   body,prohibit,tag   head,prohibit,all   title,prohibit,all   layer,prohibit,all  form,allow,all   applet,allow,all   script,allow,all   blink,allow,tag  base,prohibit,tag





## </BLOCKQUOTE>

#==============================================================
#
#--- x <A NAME="visual">Visual Effects</A> <BLOCKQUOTE>
#
#--- BODY_BG    Background Color
#
##   This switch appears in all CML files as:
##        &lt;BODY $(body_bg)&gt;
##   and is meant to provide a convenient way to encode the body
##   background or color (or other options), as in either of:
##
##        <P>     BGCOLOR="#FFFFFF"
##        <BR>    BACKGROUND=http://yourhost.com/~caucus/wbackg1.jpg

#TYPE TEXT Default body background
set body_bg  BGCOLOR="#FFFFFF"

## <P>Some pages have areas with a contrasting background color for
## highlighting information.  This should typically be set to a color
## close to the default background above.

#TYPE TEXT Default body highlight
set body_fg  BGCOLOR="#DDDDDD"

#--------------------------------------------------------------
#
#--- TOOLBAR       Toolbar Properties
#
#TYPE TEXT Toolbar background color
set tool_color  #FFFFFF

#TYPE TEXT Toolbar border width
set tool_border  BORDER=0

#--------------------------------------------------------------
#
#--- COMPASS_ARROW  Compass Bar Arrows
#
#TYPE TEXT Arrows between pages on compass bar
set compass_arrow  <B>=></B>

#--------------------------------------------------------------
#
#--- DEFAULT_IFRAME_WIDTH  IFRAME width
#
#TYPE TEXT Default width of IFRAMEs
set default_iframe_width 85%

#--------------------------------------------------------------
#
#--- IFRAME_HEIGHT  IFRAME height
#
#TYPE TEXT Default height of IFRAMEs
set default_iframe_height 300px

## </BLOCKQUOTE>
#


#==============================================================
#                            END OF SWITCH.I
#==============================================================
