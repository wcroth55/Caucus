#
#---RLIBDEL.I    Delete a "Response" LIB directory
#
# Input:
#   $inc(1)        conf num
#   $inc(2)        item num
#   $inc(3)        resp num
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 06/08/06 Rewrite for C5.1
#--------------------------------------------------------------

#set  olddir $caucus_lib()/%lowname($inc(1))/$userid()/\
#                          %digit4($inc(2))%digit4($inc(3))

if $not_empty($inc(2))
   set  olddir $caucus_lib()/ITEMS/$inc(2)
   if $greater ($inc(3) 0)
      set olddir $(olddir)/$inc(3)
   end
   eval $deldir ($(olddir))
end
