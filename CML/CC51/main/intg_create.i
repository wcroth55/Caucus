#
#--- intg_create.i
#    Sample file for automatically creating an "integrated" user.
#    See (article).  Note that this is a **template** for building
#    your own integration script.
#
#    Invoked (typically from center.cml) as
#       include $(main)/intg_create.i  userid  encpass
#
#    Parameters:
#       userid     user's caucus userid
#       encpass    if appropriate, user's encrypted password
#
#    Purpose:
#       Make sure a Caucus user has been "integrated" with the outside app.
#
#    How it works:
#       Checks to see if user has been integrated by checking if the 
#       appropriate user variable has been set.  If not, passes off the 
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
   if $empty ($user_data ($inc(1) integration))
      set lastname  $user_info ($inc(1) lname)
      set firstname $user_info ($inc(1) fname)
      eval $xshell (0 0 0 $caucus_path()/intg_create "$inc(1)" "$(firstname)" \
                    $(lastname) $inc(2) "%per_phone($inc(1))")
      eval $set_user_data ($inc(1) integration registered)
   end
end
