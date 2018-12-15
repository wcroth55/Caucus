#
#---instructors.i  Implementation of %instructors() macro.
#
#   Returns list of instructor userids for conference $inc(1).
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 06/07/04  New file.
#----------------------------------------------------------------------------

set _cname $cl_name ($inc(1))

set _instructors
set _fd $open ($caucus_path()/GROUPS/conf_ins_$(_cname) r)
while $readln ($(_fd) _line)
   set _c $str_sub (0 1 $(_line))
   if $tablefind ($(_c) # <)
      continue
   end
   set _instructors $(_instructors) $word (1 $(_line))
end
eval $close ($(_fd))

"$(_instructors)
#----------------------------------------------------------------------------

