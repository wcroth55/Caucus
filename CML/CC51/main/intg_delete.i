#
#--- intg_delete.i
#    Sample file for automatically deleting an "integrated" user.
#    See (article).  Note that this is a **template** for building
#    your own integration script.
#
#    Invoked as
#       include $(main)/intg_delete.i  userid
#
#    Parameters:
#       userid     user's caucus userid
#
#    Purpose:
#       Delete a Caucus user from the outside app.
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
#: CR 01/17/03 New file.
#----------------------------------------------------------------------------

if $(integration)
#  eval $append (/tmp/nova.log Delete: $inc(1) $newline())
   eval $xshell (0 0 0 $caucus_path()/intg_delete "$inc(1)")
end
