#
#---sortbylast.i    Sort a list of userids by person's last name.
#
#   inc(1)    (input)  name of variable containing original list of userids
#   inc(2)    (output) name of variable *to* contain sorted list
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  2/09/04 New file.
#----------------------------------------------------------------------

#---Build sortable list of quoted "lastname fullname" elements.
set lastnames
for uid in $($inc(1))
   set fullname  %per_name($(uid))
   set lastnames $(lastnames) \
      $quote ($lower ($user_info ($(uid) lname) $(fullname)))
end

#---Sort it (actually, create sorted list of indexes to original list)
set sort_indices $gen_sort (1 $(lastnames))

#---Step through sorted result in order, to build new _list of userids.
#   (Also removes duplicate userids in case they crept in somehow.)
set _list
set _prev ___
for gdex in $(sort_indices)
   set _this $word ($(gdex) $($inc(1)))
   if $not_equal ($(_prev) $(_this))
      set _list $(_list) $word ($(gdex) $($inc(1)))
      set _prev $(_this)
   end
end

#---Put final result in output variable.
set $inc(2) $(_list)
#----------------------------------------------------------------------
