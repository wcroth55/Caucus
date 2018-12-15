#
#--- intg_encpass.i
#    Sample file for getting encrypted password for "integrated" users.
#    See (article).  Note that this is a **template** for building
#    your own integration script.
#
#    Invoked (typically from startup.i) as
#       include $(main)/intg_encpass.i  userid  variable
#
#    Parameters
#       userid     Caucus userid
#       variable   name of variable to put encrypted password into.
#
#    Purpose:
#       Put the value of the encrypted password into named variable.
#
#    How it works:
#       Not all methods of integration will even use the encrypted password.
#       However, for those that do, the "outside" app typically uses the
#       encrypted Caucus password as if it were the app's users' real
#       password.  In that case, we have to get the encrypted password
#       to pass on to the outside app.  Clumsy, but it works.
#
#       In this case, we just read it directly from the passwd-style
#       flat file that apache keeps.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 01/14/02 15:30 New file.
#----------------------------------------------------------------------------

if $(integration)
#  set $inc(2) $word (2 $replace (: 32 \
#                $xshell (1 1 1 grep "^$inc(1):" $caucus_path()/caucus_passwd)

   set $inc(2) $word (2 $replace (: 32 \
                 $xshell (1 1 1 $caucus_path()/encpw $inc(1))))
end
