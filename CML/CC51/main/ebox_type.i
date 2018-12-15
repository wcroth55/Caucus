#
#---ebox_type.i    Decide on type of ebox to use.
#
#   Input variables:
#      ebox             User's preferred choice of editor
#      is_explorer55    Is this browser IE 5.5 or higher?
#      $inc(1)          If true (1), force to <textarea>
#
#   Output variables:
#      ebox_rt          1 if richtext WYSWYG editor   should be used
#      ebox_ek          1 if applet Ekit              should be used
#      ebox_ta          1 if ordinary HTML <TEXTAREA> should be used
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 01/08/03 New file.
#: CR 10/15/05 Turn off applet for now (may return some day!)
#: CR 08/20/09 Make RTE default.
#: CR 08/23/09 But fix bug to still allow(!) plaintext
#----------------------------------------------------------------------------

set ebox_rt 0
set ebox_ek 0
set ebox_ta 0

if $not ($(use_javascript))
   set ebox_ta 1
end
elif 0$inc(1)
   set ebox_ta 1
end

elif $equal (x$(ebox) xTextbox)
   set ebox_ta 1
end

elif $(can_use_richtext)
   set ebox_rt 1
end
else
   set ebox_ta 1
end
#----------------------------------------------------------------------------
