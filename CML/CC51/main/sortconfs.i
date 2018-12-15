#
#---sortconfs.i    Sort personal conference list if desired.
#
#   inc(1)   userid.   Use $userid() if none supplied.
#
#  Checks to see if the user has asked to keep their conf list sorted, and
#  does the sort, if so.  Should be included whenever the conf list is
#  changed.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 05/25/2004 Add inc(1), document.
#-----------------------------------------------------------

set _uid $word (1 $inc(1) $userid())
if $user_data ($(_uid) sort_all)
   set list $lower($user_data ($(_uid) my_confs))
   set sort $gen_sort(1 $(list) )
   set newlist
   for index in $(sort)
      set newlist $(newlist) $word($(index) $(list))$newline()
   end
   eval $set_user_data($(_uid) my_confs $(newlist))
end

