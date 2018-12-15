#
#---sortedconfs.i   Get sorted list of all conferences
#
#   Returns a list of pairs of (conference number, conference name)
#   in variable sorted_confs.  The list is sorted by conference
#   name.  A few conference names may be truncated to minimize
#   the width of the data (e.g. for use in a <select> pull-down).
#
#   Uses a global variable sorted_confs_age to refresh
#   the data no more frequently than every 10 minutes.
#
# 08/26/03 CR New file.
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#---------------------------------------------------

if $greater ($time() $plus (0$(sorted_confs_age) 600))
   set all_confs $cl_list()
   set sorted_confs

   #---Calculate 2nd longest conf name...
   set len1 0
   set len2 0
   for cnum in $(all_confs)
      set len $strlen ($cl_name ($(cnum)))
      if $greater ($(len) $(len1))
         set len2 $(len1)
         set len1 $(len)
      end
      elif $greater ($(len) $(len2))
         set len2 $(len)
      end
   end

   #---and use it to truncate conf name display.
   set len2 $plus ($(len2) 1)
   for cnum in $(all_confs)
      set name $cl_name ($(cnum))
      if $greater ($strlen($(name)) $(len2))
         set name $str_sub (0 $(len2) $(name))...
      end
      set sorted_confs $(sorted_confs) $(cnum) $(name)
   end
   set sorted_confs_age $time()
end
