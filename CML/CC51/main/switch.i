#
#---SWITCH.I    Define Caucus "switches" for this interface.
#
#   There are several CML variables which are used as "switches"
#   to control exactly how a particular Caucus interface works.
#
#   To change these switches, copy the relevant switches to
#   the file localswitch.i in the override directory main.1 
#   (create the directory and the file if necessary), and edit them there.
#
#   DO *NOT* CHANGE *THIS* FILE.
#
#  Revision history:
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  1/31/96 12:52 1.0     release version
#: CR  6/16/96 23:29 3.0/i02 release version
#: CR 10/21/96 15:23 3.1/i04 release version
#: CR  2/14/96 18:52 3.1/final release version
#: CR  6/09/99 17:50 4.09 add use_javascript
#: CR 11/02/99 14:25 Move vars here from startup.i (img, etc.)
#: CR  4/05/00 15:48 Get sweb from swebd.conf if possible.
#: CR  1/14/02 15:50 Add integration.
#: CR  3/01/02 11:00 Rearrange to use with override directories.
#: CR  8/16/02 Remove 'sweb', use GIF44.
#: CR 01/12/03 Add ebox stuff (richtext).
#==============================================================
#
#---USE_JAVASCRIPT    OK to use javascript for some features?
#
#   Some Caucus features require Javascript in order to work
#   properly.  If your site cannot, for some reason, use Javascript,
#   set use_javascript to 0.  In that case, Caucus will do its
#   best to work without Javascript.  (Some features may be disabled,
#   but all of the basic features should work.)

set use_javascript 1

#==============================================================
#
#---HTTP      Actual name of hypertext protocol in use.
#
#   Site protocol choice is controlled by the "Config http" parameter in
#   swebd.conf.  If it is not present, use 'httpdefault'.

set httpdefault http
set http $word (1 $config(http) $(httpdefault))

#---Or you may explicitly override it here for a particular interface.
#set http https

#==============================================================
#
#---IMG    Relative URL for images in this interface.

set img    $(http)://$host()/~$caucus_id()/GIF50

#==============================================================
#
#---ICON   Relative URL for downloaded images in this interface

set icon   $(img)

#==============================================================
#
#---RICHTEXT  Relative URL for richtext editor in this interface

set richtext  $(http)://$host()/~$caucus_id()/RT44

#==============================================================
#
#---EKIT      Relative URL for ekit applet editor in this interface

set ekit      $(http)://$host()/~$caucus_id()/EK44

#==============================================================
#
#---CONFHOME     List of customized "conference home" pages.
#
#   Put definitions of other "confhome" pages here; for example:
#
#   set confhome 0 Conference&nbsp;home confhome.cml \
#                1 Book&nbsp;Store      bookstore.cml

set confhome 0 Conference&nbsp;home confhome.cml

#set confhome 0 Conference&nbsp;home confhome.cml \
#             1 Book&nbsp;Store      bookstore.cml

#==============================================================
#
#---INTEGRATION.   Turn on userid integration with other web apps
#
#   This flag enables the intg*.i pages that are sample **templates**
#   for a way to integrate Caucus userids with other web apps.
#   Read those files and the relevant tech articles at caucuscare.com
#   first before changing this value!!

set integration 0

#==============================================================
#                            END OF SWITCH.I
#==============================================================
