#
#---announcedef.i   Definition of %announce macro.
#
#   announce (startdate starttime stopdate stoptime force audience... )
#
#   inc(1)  1 means define active form of macro that writes conf vars
#           0 means define no-op form of macro.
#   inc(2)  item owner userid
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
# 05/16/03 New macro file.
# 10/18/07 Correct for 5.1.
#---------------------------------------------------------------------

if 0$inc(1)
   eval $mac_define (announce \
      \$set (ann_set_var \ 
               $inc(2) \$epoch(@1 @2) \$epoch (@3 @4) @5 \$rest (6 @@)))
end
else
   eval $mac_define (announce)
end
















