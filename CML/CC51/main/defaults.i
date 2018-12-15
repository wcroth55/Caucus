#
#---DEFAULTS.I    Default Caucus "switches" for this interface.
#
#   There are several CML variables which are used as "switches"
#   to control exactly how a particular Caucus interface works.
#
#   This file contains the default values for these switches,
#   plus the default definitions of many images and icons used
#   throughout Caucus.
#
#           * * *   DO NOT CHANGE THIS FILE!!   * * *
#
#   If you wish to change the default values of these switches or
#   icons, copy the relevant definitions to the file localdefaults.i
#   in the override directory main.1 (create the directory and the
#   file if necessary), and edit them there.
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
#: CR  4/18/00 15:09 Use attach button for src_upload.
#: CR 10/13/01 21:31 Add print-entire-conf/item button (based on Gabor's work)
#: CR  3/01/02 11:00 Rearrange for use with override directories.
#: CR 10/09/02 Remove icons to icons.i file
#: CR 01/15/03 Add default_wordwide, default_wordhigh.
#: CR 12/17/04 Add base to tag_list.
#: CR 08/20/09 Change default conference tag_list to disallow javascript.
#==============================================================
#
#---GUESS_EMAIL   use this hostname for "guessed" e-mail addresses
#
#   Clicking on a user's name shows, among other things, their e-mail
#   address.  If no e-mail address was supplied by the user, Caucus
#   can "guess" that their address is "userid@" followed by the
#   value of guess_email.
#
#   Leave guess_email blank to turn off the guessing feature.

set guess_email

#==============================================================
#
#---DEFAULT_COLS        width of text-entry box
#
#   Default width (in single-width characters) of all item, response, etc.
#   text entry boxes.

set default_cols 60

#==============================================================
#
#---DEFAULT_ROWS        height of text-entry box
#
#   Height (in single-width characters) of all item, response, etc.
#   text entry boxes.

set default_rows 8

#==============================================================
#
#---DEFAULT_WORDWIDE    width of word-processor box
#
#   Default width (in pixels) of all item, response, etc. word processor boxes

set default_wordwide 520

#==============================================================
#
#---DEFAULT_WORDHIGH    height of word-processor box
#
#   Default height (in pixels) of all item, response, etc. word processor boxes

set default_wordhigh 150

#==============================================================
#
#---DEFAULT_PAGERESPS   number of responses that appear in a single page
#
#   In order to keep page load times short, long items are broken up
#   into multiple pages of a certain # of responses each.  This is
#   the default value for that number.  It may be overridden by the user.

set default_pageresps 100

#==============================================================
#
#---DEFAULT_CONTEXT     number of responses before a selected response
#
#   When a user views a particular response (such as the first new response
#   in an item), a certain number of immediately previous responses are
#   also loaded into the page to provide "context".  This is the default
#   value for that number.  It may be overridden by the user.

set default_context    3

#==============================================================
#
#---DEFAULT_LMARGIN     size of left margin when viewing items
#
#   This must a simple number, such as "20", which means number of pixels;
#   or a percentage, such as "8%", which means percentage of the width
#   of the screen.

set default_lmargin  2\%

#==============================================================
#
#---DEFAULT_RMARGIN     size of right margin when viewing items
#
#   This must a simple number, such as "20", which means number of pixels;
#   or a percentage, such as "8%", which means percentage of the width
#   of the screen.

set default_rmargin  2\%

set default_thumbnail 60x40

#==============================================================
#
#---JAPANESE_SEARCH     Effect of language on "search items" interface.
#
#   For the Japanese langauge version of Caucus, this should be set to 1.
#   It "turns off" the display (in search.cml) of the question about
#   searching for matches at the beginning vs. in the middle of words,
#   and assumes the answer is "in the middle of words".

set japanese_search    0

#==============================================================
#
#---BODY_BG    Default body background
#
#   This switch appears in all CML files as:
#       "<BODY $(body_bg)>
#   and is meant to provide a convenient way to encode the body
#   background or color (or other options), as in either of:
#
#        set body_bg     BGCOLOR="#FFFFFF"
#        set body_bg     BACKGROUND="$(icon)/wbackg1.jpg"

set body_bg  BGCOLOR="#FFFFFF"

#==============================================================
#
#---DOWNLOADABLE_ICONS  Allow local source of icon images?
#
#   Advanced users may choose to put copies of the Caucus icons
#   on their local machine.
#
#   A value of "1" allows using downloadable icons; "0" ignores them.

set downloadable_icons 0

#==============================================================
#
#---CONFIRM_PASSWORD     Require password confirmation on Caucus startup?
#
#   Caucus can require confirmation of a user's password before allowing
#   entry into the conferences.  This is particularly useful in academic
#   environments, where PCs may be shared by many people and Web browsers
#   may be left running -- with the previous user's id and password still
#   cached in the browser!
#
#   A value of "1" enables password confirmation, "0" disables it.

set confirm_password 0

#==============================================================
#
#---CHANGE_PASSWORD     Are users allowed to change their own password?
#
#   A value of "1" enables password changing, "0" disables it.

set change_password 1

#==============================================================
#
#---EDITABLE          let users edit their own responses?
#
#   Normally users may edit (change) their own responses at any time.
#   Some sites object to thus "being able to change history".
#
#   A value of "1" enables response editing, "0" disables it.

set editable  1

#==============================================================
#
#---BETWEEN_RESPONSES       what divider appears between responses.

set between_responses <HR>

#==============================================================
#
#---Log debugging information?
#
#   Should a problem occur, the Caucus technical support representative
#   may request that you "turn debugging on".  To do so, change "$debug(0)"
#   to "$debug(1)".

eval $debug(0)

#==============================================================
#
#---SHOW_ID        Display "(userid") after person's name?
#
#   Whenever a response is displayed, the author's name appears with
#   it.  You may also force Caucus to display the author's userid
#   as well, to discourage "spoofing" of responses.
#
#   A value of "1" enables userid display, "0" disables it.

set show_id 0

#==============================================================
#
#---MAIL_OUT    Enable conference participation via e-mail?
#
#   Users without web access may participate in a conference via e-mail.
#   New material from a conference may be sent to them via e-mail, and
#   their replies to that mail can be automatically integrated into
#   a conference.
#
#   NOTE!!
#   To enable e-mail participation, a special package of e-mail CML
#   scripts must be installed in a userid dedicated to this purpose.
#
#   Set mail_out to be the userid that in which this package was
#   installed.  This enables sending new material *to* such e-mail
#   participants.  This displays a link on each conference's
#   customization page, which the organizer can use to add e-mail
#   addresses to "subscribe" to the conference.
#
#   Leave mail_out blank (empty) to disable this feature.

set mail_out

#==============================================================
#
#---CENTER_NAME    Name of "Caucus Center".
#
#   Many sites may wish to change the name of the "Caucus Center" to
#   reflect the name or purpose of their site.  All references to this
#   page have been collected here for ease of customization.
#

set center_name    Caucus&nbsp;Center

#==============================================================
#
#---TOOLBAR       Cosmetic properties of Caucus toolbar
#
set tool_color #FFFFFF
set tool_border BORDER=0

#==============================================================
#
#---DEF_ROWCOLOR1 & 2  row colors for filesafe
#
set def_rowcolor1  #C0FFFF
set def_rowcolor2  #FFFFC0

#==============================================================
#
#---COMPASS_ARROW  Arrows between pages on compass bar
#
set compass_arrow   <B>=&gt;</B>

#==============================================================
#
#---HELP_BUTTONS   Enable browser buttons on pop-up HELP window.
#
set help_buttons   0

#==============================================================
#
#---MANAGE_MAXCONFS   Maximum number of conferences displayed
#                     at once in management functions.  If the
#                     total number of conferences exceeds this
#                     some functions revert to "one at a time"
#                     mode.
#
set manage_maxconfs 25

#==============================================================
#
#---EDIT_TIMES   Use this variable to change the lengths of time
#                that you have to choose from when limiting
#                users' ability to edit their own responses.
#
#                Times are in seconds.  For example:
#                    5 min  =  300
#                   15 min  =  900
#                    1 hr   =  3600
#                    1 day  =  86400
#                    1 week =  604800
#
# Be sure to format your changes or additions just like this example,
# ending each option with a | character.
#
set edit_times  300   = up to 5 minutes | \
                3600  = up to 1 hour | \
                86400 = up to 1 day |


#==============================================================
#
#---TAG_LIST	Use this variable to define a set of tags that
#               organizers should be able to allow or disallow
#               in postings. Disallowed tags are not displayed
#               when the posting is viewed, but they remain in
#               the text.
# 
#               Each entry consists of three parts, in the form
#                  tag,prohibit|allow,all|tag
#
#               No spaces are allowed between commas.  The first
#               element is the name of the tag, the second is a
#               switch determining whether the tag is allowed or
#               prohibited, and the third is a switch determining
#               whether the text within the tag is also hidden,
#               or just the tag.
# 
#               For instance, the string "form,prohibit,all"
#               would suppress display of all <FORM> tags and the
#               text between them.
#
#               These restrictions apply to users and organizers.
#               Individual conference organizers can set
#               conference-specific permissions for both users
#               and organizers.
#
#------------------------------------------------------------------------

set tag_list \
    html,prohibit,tag \
    body,prohibit,tag \
    head,prohibit,all \
    title,prohibit,all \
    form,allow,all \
    applet,prohibit,all \
    script,prohibit,all \
    blink,prohibit,tag, \
    base,prohibit,tag

#==============================================================
#
#   Other variables added later...

set allow_copy    1
set body_fg       BGCOLOR="#DDDDDD"
set cc_allowall   1
set cc_customize  1
set cc_enotify    0
set cc_join       1
set cc_pcl        1
set cmlerr_def   no
set confirm_join  0
set force_join_personal 1
set e_notify      0
set en_run        0
set goodbye_url

set default_iframe_height  300px
set default_iframe_width    85%
set user_limit_trigger      50

#==============================================================
#                            END OF DEFAULTS.I
#==============================================================
