#
#--- local_prereg.i   Local changes during registration (pre registration)
#
#   Arguments:
#      inc(1)     userid
#
#   Purpose:
#      Allow for local changes to the registration process (prior to
#      actual creation of the userid.)
#
#   How it works
#      DO NOT CHANGE THIS FILE!  Read up on local changes and the
#      Caucus overrideable directories (see the CML reference guide).
#
#      Copy this file to your reg.1 (or equivalent) directory, and
#      edit it there.
#
#      The new file may then contain any CML statements needed to
#      perform local registration actions.  Note that the userid
#      doesn't actually exist at the time this file is run -- but
#      the userid *about* to be created is supplied in $inc(1).
#
#   Notes:
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/18/02 New file.
#: CR 12/03/04 Remove example 'add to group', as it is now always done.
#==========================================================================


#==========================================================================
