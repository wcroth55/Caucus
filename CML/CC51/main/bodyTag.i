#
#---bodyTag.i   Just the <body> tag and its options.
#
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 09/04/09 Extracted from old body.i
#------------------------------------------------------------------

if   $equal ($inc(1) x)
   set cencolor $site_data(0 - if_$(iface)_cencolor)
   if $empty ($(cencolor))
      "<BODY $(body_bg)                  $inc(2) $inc(3) $inc(4) $inc(5)>
   end
   elif $equal (x$str_sub (0 1 $(cencolor)) x#)
      "<BODY bgcolor="$(cencolor)"       $inc(2) $inc(3) $inc(4) $inc(5)>
   end
   else
      "<BODY background="$(cencolor)"    $inc(2) $inc(3) $inc(4) $inc(5)>
   end
end

elif $empty ($site_data ($inc(1) - conf_colors))
   "<BODY $(body_bg)                     $inc(2) $inc(3) $inc(4) $inc(5)>
end

else
   "<BODY $site_data($inc(1) - conf_colors) $inc(2) $inc(3) $inc(4) $inc(5)>
end
