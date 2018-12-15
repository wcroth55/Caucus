#
#---statwizopen.i    "Open" output for stat wizard
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 07/26/03 New script.
#: CR 10/13/04 Use CSS.
#------------------------------------------------------------------------

if $equal ($form(restype) w)
   "Content-type: text/html
   "
   "<html>
   "<head>
   include $(main)/css.i
   "</head>
   "<body bgcolor="#FFFFFF">
   if $not_empty ($inc(1))
      "<h2>$inc(1) $inc(2) $inc(3)</h2>
   end
   "<table border=0>
end
else
   set spreadsheet

   set pid  $word (1 $replace (/ 32 $pid()))
   set base $caucus_path()/public_html/TEMP/$(pid)
   set fd   $open ($(base).tmp w)
end
