#
#---macros.i  (registration area)
#
#   Define any macros in here that are needed during the registration
#   process.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/18/02 New file.
#: CR 11/30/03 Add *#%$! missing ")" to end of add_to_group definition.
#: CR 10/13/04 Define urcregnums.
#----------------------------------------------------------------------
#
#     %leave(url)  terminate caucus process and goto URL
eval $mac_define (leave leave.cml?@@)

#---%add_to_group (groupname uid [access])
eval $mac_define (add_to_group \
                  \$append ($caucus_path()/GROUPS/@1 @2 @3\$newline()))

#---%urcregnums()
#   Sorted list of field numbers of REG urc field variables, e.g.
#   REG_CC44/reg_1 is "1", etc.
eval $mac_define (urcregnums \$includecml(\$(main)/urcregnums.i))

eval $mac_define (per_name  \$user_info (@1 fname) \$user_info (@1 lname))

#-----------------------------------------------------------------------
