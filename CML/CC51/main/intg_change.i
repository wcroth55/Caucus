#
#--- intg_change.i
#    Sample file for automatically changing info for an "integrated" user.
#    See (article).  Note that this is a **template** for building
#    your own integration script.
#
#    Invoked as
#       include $(main)/intg_change.i  userid  encpass
#
#    Parameters:
#       userid     user's caucus userid
#       encpass    if appropriate, user's encrypted password
#
#    Purpose:
#       Change a Caucus user's personal info in the outside app.
#
#    How it works:
#       Passes off the 
#       personal information to a matching shell script, which executes some 
#       appropriate program (possibly a curl script to post to a URL).
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 01/14/02 15:30 New file.
#----------------------------------------------------------------------------

if $(integration)
   set lastname  $user_info ($inc(1) lname)
   set firstname $user_info ($inc(1) fname)
   eval $xshell (0 0 0 $caucus_path()/intg_change "$inc(1)" "$(firstname)" \
                 $(lastname) $inc(2) "%per_phone($inc(1))")
end
