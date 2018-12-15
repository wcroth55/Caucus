#
#---columnfield.i   Used repeatedly inside man_ucreated.cml
#
#   Input:
#      column    (previous column number)
#      line      (raw data)
#
#   Output
#      column    (incremented by 1)
#      field     (field value from line)
#
#   Increment column by 1
#   Pluck line # 'column' out of $(line)
#   Strip trailing newline, and remove any surrounding double quotes.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This file is *not* open source, and is part of the "batch registration"
# add-on package. You must purchase a license to use this package.
#
#: CR 03/29/03 New file.
#: CR 12/08/05 Update header doc.
#---------------------------------------------------------------------

   set column $plus  ($(column) 1)
   set field  $lines ($(column) 1 $(line))

   set lastpos $minus   ($strlen ($(field)) 1)
   set lastchr $str_sub ($(lastpos) 1 $(field))
   if $equal ($(lastchr) $newline())
      set field  $str_sub (0 $(lastpos) $(field))
   end
   set field  %stripquotes ($(field))
   set field $replace (03 10 $(field))
#---------------------------------------------------------------------

