#
#---listpeople.i   Implement %listpeople() macro.
#
#   Usage: %listpeople (separator id1 [id2...])
#
#   inc(1) separator string (must not contain blanks!)
#   inc(2) $quote()'d list of userids
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 04/27/04 New file.
#---------------------------------------------------------------

#---List everyone in inc(2), but drop non-existent people or
#   duplicates.  (Duplicate matching could be even smarter...)
set prev ___
for this in $inc(2)
   if $empty (%per_lname ($(this)))
      continue
   end
   if $not_equal ($(this) $(prev))
      "%person($(this))$inc(1)
   end
   set prev $(this)
end
