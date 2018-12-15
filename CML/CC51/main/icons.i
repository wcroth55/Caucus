#
#---ICONS.I         List of image/icon files
#
#   All of the images used as icons in the interface are defined
#   below as variables, along with their HEIGHT, WIDTH, and ALT
#   elements.  (Note that including the specific HEIGHT and WIDTH
#   elements make the page effectively load faster.)
#
#   If you wish to change an image, copy the relevant line from
#   this file to localdefaults.i, and then change it there.
#
#   Note that the definitions depend on the proper setting of 'icon_type'
#   by the caller.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 05/26/2004 Final version of status, assignments buttons.
#:CR 08/20/2004 Add src_cloud icon (for "hidden").
#:CR 10/13/04   Add src_noice (blank, same size as ice).
#:CR 12/22/04   Add src_tasks
#:CR 01/24/05   Add src_dnarrow, src_uparrow.
#:CR 03/30/05   Remove toolbar buttons.
#:CR 04/13/05   C5 code merge
#:CR 08/19/09   Add src_blank (a blank bullet, as it were)
#==========================================================================

set src_logo     SRC="$(icon)/lcaucus0.$(icon_type)" \
                 WIDTH=148 HEIGHT=40 ALT="Caucus" BORDER=0
set src_toolbar2 SRC="$(icon)/toolbar2.$(icon_type)" \
                 WIDTH=190 HEIGHT=25 ALT="[Toolbar]" BORDER=0

set src_check    SRC="$(icon)/checkmrk.$(icon_type)" WIDTH=20 HEIGHT=15 ALT="Y"
set src_isnew    SRC="$(icon)/inew.$(icon_type)"  WIDTH=23 HEIGHT=12 ALT="New!"

set src_1quit    SRC="$(icon)/bred1quit.$(icon_type)" \
                 WIDTH=42  HEIGHT=47 ALT="Quit" BORDER=0
set src_print   SRC="$(icon)/bprint.$(icon_type)"  \
                WIDTH=26  HEIGHT=33  BORDER=0

set src_expand  SRC="$(icon)/bexpand.$(icon_type)"  \
                WIDTH=26  HEIGHT=33 ALT="Expand" BORDER=0
set src_expand0 SRC="$(icon)/bexpand0.$(icon_type)" \
                WIDTH=26  HEIGHT=33 ALT="" BORDER=0
set src_1stpg   SRC="$(icon)/b1stpg.$(icon_type)"   \
                WIDTH=26  HEIGHT=33 ALT="1st Pg" BORDER=0
set src_1stpg0  SRC="$(icon)/b1stpg0.$(icon_type)" \
                WIDTH=26  HEIGHT=33 ALT="" BORDER=0
set src_lastpg  SRC="$(icon)/blastpg.$(icon_type)"  \
                WIDTH=26  HEIGHT=33 ALT="Last Pg" BORDER=0
set src_lastpg0 SRC="$(icon)/blastpg0.$(icon_type)" \
                WIDTH=26  HEIGHT=33 ALT="" BORDER=0
set src_nextpg  SRC="$(icon)/bnextpg.$(icon_type)"  \
                WIDTH=26  HEIGHT=33 ALT="Next Pg" BORDER=0
set src_nextpg0 SRC="$(icon)/bnextpg0.$(icon_type)" \
                WIDTH=26  HEIGHT=33 ALT="" BORDER=0
set src_prevpg  SRC="$(icon)/bprevpg.$(icon_type)"  \
                WIDTH=26  HEIGHT=33 ALT="Prev Pg" BORDER=0
set src_prevpg0 SRC="$(icon)/bprevpg0.$(icon_type)" \
                WIDTH=26  HEIGHT=33 ALT="" BORDER=0
set src_prevpg2 SRC="$(icon)/bprevpg.$(icon_type)"  \
                WIDTH=26  HEIGHT=33 ALT="Prev Pg" BORDER=0

set src_arleft  SRC="$(icon)/barleft.$(icon_type)"  \
                WIDTH=27  HEIGHT=26 ALT="&lt;=="
set src_arrite  SRC="$(icon)/barright.$(icon_type)" \
                WIDTH=27  HEIGHT=26 ALT="==&gt;"

set src_edit    SRC="$(icon)/bedit.$(icon_type)"    \
                WIDTH=26  HEIGHT=33 ALT="Edit" BORDER=0
set src_pass    SRC="$(icon)/bpass.$(icon_type)"    \
                WIDTH=41  HEIGHT=33 ALT="Pass" BORDER=0
set src_upload  SRC="$(icon)/battach.$(icon_type)"  \
                WIDTH=30  HEIGHT=34 ALT="Attach" BORDER=0

set src_compass SRC="$(icon)/icompass.$(icon_type)" WIDTH=44  HEIGHT=44 ALT="*"

set src_ice     SRC="$(icon)/iice.$(icon_type)" WIDTH=15 HEIGHT=17 ALT="F"
set src_noice   SRC="$(icon)/clearpix.gif"      width=15 height=1
set src_cloud   SRC="$(icon)/icloud.gif"        WIDTH=11 HEIGHT=11 
set src_retired SRC="$(icon)/icon_z.gif"        WIDTH=16 HEIGHT=17 ALT="R"
set src_trash   SRC="$(icon)/itrash.gif"        WIDTH=13 HEIGHT=15 ALT="T" border=0
set src_trash2  SRC="$(icon)/itrash2.gif"       WIDTH=11 HEIGHT=13 ALT="Delete"
set src_iedit   SRC="$(icon)/iedit.gif"         WIDTH=15 HEIGHT=14 ALT="Edit"
set src_icheck  SRC="$(icon)/icheck.gif"        WIDTH=9  HEIGHT=11 ALT="Check"
set src_wiki    SRC="$(icon)/iwiki.gif"         WIDTH=16 HEIGHT=16 ALT="W"
set src_blog    SRC="$(icon)/iblog.gif"         WIDTH=16 HEIGHT=16 ALT="B"
set src_newrow  SRC="$(icon)/inewrow.gif"       WIDTH=18 HEIGHT=14 \
                                                ALT="New Row" border=0
set src_key     SRC="$(icon)/ikey2.gif"         WIDTH=13 HEIGHT=15 
set src_symbols SRC="$(icon)/bsymbols.gif"      WIDTH=32 HEIGHT=34 ALT=""
set src_lfarrow SRC="$(icon)/ilfarrow.gif"  WIDTH=9 HEIGHT=10 ALT="" border=0
set src_rtarrow SRC="$(icon)/irtarrow.gif"  WIDTH=9 HEIGHT=10 ALT="" border=0
set src_dnarrow SRC="$(icon)/idnarrow.gif"  WIDTH=10 HEIGHT=9 ALT="Dn" border=0
set src_uparrow SRC="$(icon)/iuparrow.gif"  WIDTH=10 HEIGHT=9 ALT="Up" border=0

set src_balloon SRC="$(icon)/iballoon.gif"      WIDTH=12 HEIGTH=12 ALT=""
set src_quiz    SRC="$(icon)/iquiz.gif"         WIDTH=15 HEIGHT=14 ALT="Q"

set src_bullet  SRC="$(icon)/bullet.$(icon_type)" WIDTH=10 HEIGHT=10 ALT="*"
set src_blank   SRC="$(icon)/clearpix.gif"        WIDTH=10 HEIGHT=10 

set src_eboxtl  SRC="$(icon)/tblue_tl.gif" WIDTH=19 HEIGHT=19 ALT="/"
set src_eboxtr  SRC="$(icon)/tblue_tr.gif" WIDTH=20 HEIGHT=19 ALT="\"
set src_eboxbl  SRC="$(icon)/tblue_bl.gif" WIDTH=19 HEIGHT=19 ALT="\"
set src_eboxbr  SRC="$(icon)/tblue_br.gif" WIDTH=20 HEIGHT=19 ALT="/"
set src_blpix   SRC="$(icon)/bluepix.gif"  ALT=""
set eboxbg      bgcolor="#A0DDFF"

set src_quiznot  SRC="$(icon)/circleBlack.gif"  WIDTH=16 HEIGHT=16 ALT=""
set src_quizhalf SRC="$(icon)/circleOrange.gif" WIDTH=16 HEIGHT=16 ALT="I"
set src_quizdone SRC="$(icon)/circleGreen.gif"  WIDTH=16 HEIGHT=16 ALT="D"

set src_buynow   SRC="$(icon)/iBuyNowPayPal.gif" WIDTH=62 HEIGHT=31 ALT="Buy Now"

set src_globe    SRC="$(icon)/iglobe.png" WIDTH=23 HEIGHT=23 ALT="GeoLoc"
set src_calendar SRC="$(icon)/icalendar.gif" WIDTH=16 HEIGHT=16 ALT="Calendar"

set src_clear   SRC="$(icon)/clearpix.gif" ALT=""

#==========================================================================
