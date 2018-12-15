#
#---urlregget.i       Get a GET/PUT parameter value.
#
#   Invoked as:
#      include $(regis)/urlregget.i  parmname
#
#   Purpose:
#      Sets variable 'parmname' to the value of the PUT or GET http parameter 
#      of the same name.  (Tried PUT first, if empty, then tries GET).
#      GET values are parsed out of the input string 'querystr'.
#
#   Notes:
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 05/04/03 New script.
#--------------------------------------------------------------------------

#---Try PUT first.
set vname $inc(1)
set $(vname)
if $not_empty ($form ($inc(1)))
   set $(vname) $form($inc(1))
   return
end

#---If empty, then look through 'querystr' for a match.
for x in $(querystr)
   if $equal ($str_index ($(vname)= $(x)) 0)
#     "x='$(x)'
      set $(vname) $url_decode($str_sub ($strlen($(vname)=) 5000 $(x)))
#     "vname=$(vname)='$($(vname))'
#     "urlregget.i: urlregpw='$(urlregpw)'
      break
   end
end
#--------------------------------------------------------------------------
