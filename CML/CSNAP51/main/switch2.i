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
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
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

#==============================================================
#
#--- DEFAULT_COLS        Text Entry Box Parameters
#
##   Default width and height (in single-width characters) of all item,
## response, etc. text entry boxes.

#TYPE TEXT Width of text-entry box
set default_cols 60

#TYPE TEXT Height of text-entry box
set default_rows 8

#==============================================================
#
#--- DEFAULT_PAGERESPS   Responses Per Page
#
##   In order to keep page load times short, long items are broken up
##   into multiple pages of a certain # of responses each.  This is
##   the default value for that number.  It may be overridden by the user.

#TYPE TEXT Number of responses that appear in a single page
set default_pageresps 25

#==============================================================
#
#--- DEFAULT_CONTEXT     Response Context
#
##   When a user views a particular response (such as the first new response
##   in an item), a certain number of immediately previous responses are
##   also loaded into the page to provide "context".  This is the default
##   value for that number.  It may be overridden by the user.

#TYPE TEXT Number of responses before a selected response
set default_context 3

#==============================================================
#
#--- DEFAULT_LMARGIN,RMARGIN     Item Display Margins
#
##   This must be a simple number, such as "20", which means number of pixels;
##   or a percentage, such as "8%", which means percentage of the width
##   of the screen.

#TYPE TEXT Size of left margin when viewing items
set default_lmargin 0

#TYPE TEXT Size of right margin when viewing items
set default_rmargin 0

#==============================================================
#
#--- JAPANESE_SEARCH   Japanese Searching
#
##   For the Japanese language version of Caucus, this should be on.
##   It "turns off" the display (in search.cml) of the question about
##   searching for matches at the beginning vs. in the middle of words,
##   and assumes the answer is "in the middle of words".

#TYPE CHECKBOX Use Japanese search settings?
set japanese_search 0

#==============================================================
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
set body_bg BGCOLOR="#FFFFFF"

## <P>Some pages have areas with a contrasting background color for
## highlighting information.  This should typically be set to a color
## close to the default background above.

#TYPE TEXT Default body highlight
set body_fg BGCOLOR="#EDEDED"

#==============================================================
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
set confirm_password 0

#==============================================================
#
#--- CHANGE_PASSWORD     Password Changing
#
#   A value of "1" enables password changing, "0" disables it.

#TYPE CHECKBOX Are users allowed to change their own password?
set change_password 1

#==============================================================
#
#--- BETWEEN_RESPONSES       Response Divider

#TYPE TEXT Divider that appears between responses
set between_responses <HR>

#==============================================================
#
#--- DEBUGGING      Debugging
#
##   Should a problem occur, the Caucus technical support representative
##   may request that you "turn debugging on".  To do so, change "$debug(0)"
##   to "$debug(1)".

#TYPE TEXT Log debugging information?
set ignore $debug(0)

#==============================================================
#
#--- SHOW_ID       Userid Display
#
##   Whenever a response is displayed, the author's name appears with
##   it.  You may also force Caucus to display the author's userid
##   as well, to discourage "spoofing" of responses.
#
#   A value of "1" enables userid display, "0" disables it.

#TYPE CHECKBOX Display "(userid") after person's name?
set show_id 1

#==============================================================
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

#==============================================================
#
#--- CENTER_NAME    Caucus Center
#
##   Many sites may wish to change the name of the "Caucus Center" to
##   reflect the name or purpose of their site.  All references to this
##   page have been collected here for ease of customization.
#

#TYPE TEXT Name for "Caucus Center"
set center_name Caucus Center

#==============================================================
#
#--- TOOLBAR       Toolbar Properties
#
#TYPE TEXT Toolbar background color
set tool_color #FFFFFF

#TYPE TEXT Toolbar border width
set tool_border BORDER=0

#==============================================================
#
#--- COMPASS_ARROW  Compass Bar Arrows
#
#TYPE TEXT Arrows between pages on compass bar
set compass_arrow <B>=></B>

#==============================================================
#
#--- HELP_BUTTONS   Help Window Buttons
#
#TYPE CHECKBOX Enable browser buttons on pop-up HELP window?
set help_buttons 0

#==============================================================
#
#--- ACTON_SIZE     "Act on Responses" window
#
#TYPE TEXT Size of "Act on Responses" pop-up window.
set acton_size width=330,height=350

#==============================================================
#
#--- USER_LIMIT_TRIGGER    User Limit
#
#TYPE TEXT Warn Administrator when this many user slots left
set user_limit_trigger 50

#==============================================================
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
set manage_maxconfs 25

#==============================================================
#
#--- EDITABLE          Response Editing
#
##   Normally users may edit (change) their own responses at any time.
##   Some sites object to thus "being able to change history".
#
#   A value of "1" enables response editing, "0" disables it.

#TYPE CHECKBOX Let users edit their own responses?
set editable 1

#==============================================================
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
set allow_copy 1

#==============================================================
#
#--- CC_JOIN             Default to join newly created conferences?
#
##  When a new conference is created, the creator may choose
##  to be automatically made a member of the conference.
##  Use this checkbox to define the default value (whether the
##  creator should or should not be made a member).
#
#TYPE CHECKBOX Conference creators default to become members?
set cc_join 1

#==============================================================
#
#--- CC_PCL             Default to add new conf to Pers Conf List?
#
##  When a new conference is created, the creator may choose
##  to automatically add it to their personal conference list.
##  Use this checkbox to define the default value (whether the
##  conference should be added or not).
#
#TYPE CHECKBOX Add newly created confs to Personal Conference List?
set cc_pcl 1

#==============================================================
#
#--- CC_CUSTOMIZE       Default to customize new conference immediately?
#
##  When a new conference is created, the creator may choose
##  to immediately begin customizing it.
##  Use this checkbox to define the default value (immediately
##  customize or not).
#
#TYPE CHECKBOX Add newly created confs to Personal Conference List?
set cc_customize 1

#==============================================================
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
set edit_times 300   = up to 5 minutes |    3600  = up to 1 hour |    86400 = up to 1 day






#==============================================================
#
#--- TAG_LIST   Options for prohibited tags
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
#  elements: tag,allow/deny,hide_what
#

#TYPE TEXTAREA Prohibited tag options
set tag_list html,prohibit,tag   body,prohibit,tag   head,prohibit,all   title,prohibit,all   form,allow,all   applet,allow,all   script,allow,all   blink,allow,tag






#==============================================================
#                            END OF SWITCH.I
#==============================================================




