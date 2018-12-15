#
#---sortedgroups.i   Get sorted list of all groups.
#
#   Returns list in variable sorted_groups.
#
# 08/26/03 CR New file.
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#---------------------------------------------------

include $(main)/groups.i
set all_groups $(groups)
set sort_indices    $gen_sort (1 $(all_groups))
set sorted_groups
for gdex in $(sort_indices)
   set sorted_groups $(sorted_groups) $word ($(gdex) $(all_groups))
end
