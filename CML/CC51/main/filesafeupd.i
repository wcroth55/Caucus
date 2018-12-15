#
#---filesafeupd.i   Decide if filesafe entry can be updated.
#
#   Inputs:
#      manager       is this user a filesafe manager?
#      dotname       ".xyz" file containing data about entry in filesafe
#
#   Outputs:
#      update        Can user update this file (1=yes, 0=no)
#      filesafe_err  Text of error msg if illegal op attempted
#      uid           Owner of file
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/23/02 New file, "subfunction" to filesafef.cml
#---------------------------------------------------------------------------

set filesafe_err
set uid

set update 0$(manager)

set fil $open ($(dotname) r)
if $(fil)
   eval $readln($(fil) uid)
   eval $close ($(fil))
   if $not ($(update))
      set update $equal ($(uid) $userid())
   end
   if $not ($(update))
      set filesafe_err You cannot update that file.
   end
end
else
   set update 1
   set uid $userid()
end

#---------------------------------------------------------------------------

