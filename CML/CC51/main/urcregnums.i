#
#---urcregnums.i    Implementation of %urcregnums() macro.  Evaluates
#                   to a sorted list of the "REG" urc field numbers for
#                   the current interface.
#
#   This is necessary, because in some (rare?) cases, the urc nums
#   have empty gaps -- we can't just count from 1 until the first
#   empty field.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 09/02/04 New file.
#:CR 11/24/04 Ignore vars like REG_GJ2/reg_t_18 (only get /reg_##)
#-----------------------------------------------------------------

include $(main)/urcbase.i

set _urcnums
set _urc $strlen($(urcbase))
for x in $list_site_data (0 - $(urcbase))
   set _str $str_sub ($(_urc) 100 $(x))
   if $numeric($(_str))
      set _urcnums $(_urcnums) $(_str)
   end
end
"$num_sort (1 $(_urcnums))
#-----------------------------------------------------------------
