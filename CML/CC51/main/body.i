#
#---BODY.I   Include file for <BODY> tag.
#
#   Usage:
#     include $(main)/body.i  $arg(3)   [field1  field2  field3  field4 ]
#
#     where the arguments "field1" thru "field4" are additional fields that
#     you want inside the <BODY ...> tag.
#
#     Note that for most CML files that are not "inside" a conference
#     (e.g., center.cml), arg(3) is "x", and body.i knows how to
#     interpret that.  
#
#     Files that do NOT follow the standard argument list (in which arg(3) 
#     is always "x" or the conference number) should explicitly supply an 
#     "x" in place of $arg(3) when using this include file.  (This includes 
#     many of the help sub-window files.)
#
#   Purpose:
#     To provide a simple(?) way to implement different screen backgrounds
#     for different conferences, w/o changing (more than one line of)
#     all the other CML files.  See custom.cml part IV, and custombf.cml
#     for more details.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 05/13/97 New file.
#: CR 10/09/02 Support center bkg colors, png images.
#: CR 10/24/05 Remove javascript tags around announce.i
#: CR 03/13/06 Add top banner capability.
#: CR 09/04/09 Extract actual <body> tag to bodyTag.i
#: CR 09/24/09 Add top banner center image option.
#------------------------------------------------------------------

include $(main)/bodyTag.i $inc(1) $inc(2) $inc(3) $inc(4) $inc(5)

include $(main)/leftBox.i

"<div $ifthen (0$(leftBox) id="mainBox")>

#---Banner at the top:
if $greater (0$(banner_height) 0)
   "<div style="position:absolute; top:0; left:0; width: 100%;">
   "<table border=0 cellspacing=0 cellpadding=0 width="100%"
   if $not_empty ($(banner_bg))
      "  background="$(banner_bg)"
   end
   "><tr valign="top" align="left"
   if $not_empty ($(banner_left))
      "><td 
      "  $ifthen ($not_empty ($(banner_leftwidth)) width="$(banner_leftwidth)")
      "   ><img src="$(banner_left)"
      "      vspace="0" hspace="0" align="top" border="0" alt="" ></td
   end
   if $not_empty ($(banner_center))
      "><td align=center><img src="$(banner_center)"
      "    vspace="0" hspace="0" align="top" border="0" alt=""></td
   end
   if $empty ($(banner_right))
      "><td>&nbsp;</td
   end
   else
      "><td align=right><img src="$(banner_right)"
      "    vspace="0" hspace="0" align="top" border="0" alt=""></td
   end
   "></table>
   "</div>
   "<img $(src_clear) width="5" height="$(banner_height)"><br>
end

#---Announcements
include $(main)/announce.i

#-----------------------------------------------------------------
