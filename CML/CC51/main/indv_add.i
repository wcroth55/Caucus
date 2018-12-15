#
#---INDV_ADD.I   Handle results of ADD from indvresp.cml.
#                Then close current (pop-up) window.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#-----------------------------------------------------------------------

set category $replace(32 _ $form(category))

if $equal ($(category) (Create_New_Category))
   include $(main)/indv_cnc.i
   return
end

include $(main)/indv_nbk.i
#-----------------------------------------------------------------------
