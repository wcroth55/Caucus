#
#--- local_postreg.i   Local changes during registration (post registration)
#
#   Arguments:
#      inc(1)     userid
#
#   Purpose:
#      Allow for local changes to the registration process (after to
#      actual creation of the userid.)
#
#   How it works
#      DO NOT CHANGE THIS FILE!  Read up on local changes and the
#      Caucus overrideable directories (see http://caucuscare.com/TLIB.)
#
#      Copy this file to your reg.1 (or equivalent) directory, and
#      edit it there.
#
#      This file may contain any CML statements need to finish the
#      registration process -- but there's a trick: it must *output*
#      the equivalent commands, rather than executing them.
#
#      For example, if you wanted to create a new user variable
#      called i_registered_now, containing the date and time the
#      user registered, you would write the following CML:
#
#         "eval \$set_user_data ($inc(1) i_registered_now \$dateof(\$time()))
#
#      (Note that set_user_data, dateof, and time are escaped, but
#       inc(1) is not.)
#
#      So the output of this file actually goes into another file, 
#      which is *then* in turn executed.  Strange, but it works.
#
#   Notes:
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/18/02 New file.
#==========================================================================


#==========================================================================
