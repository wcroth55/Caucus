#
#---add2persconf.i   Add to personal conf list
#
#   inc(1)  userid
#   inc(2)  conf num
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 05/25/2004 New file.
#:CR 02/08/2005 Use $co_name() instead of $cl_name() so it works in all cases!
#:CR 04/13/2005 C5 code merge.
#-----------------------------------------------------------------------------

set found 0
set myconfs $replace (10 32 $user_data ($inc(1) my_confs))
set found   $tablefind ($cl_name($inc(2)) $(myconfs))

if $not ($(found))
   eval  $set_user_data ($inc(1) my_confs \
               $(myconfs)$newline()$cl_name($inc(2))$newline())
end
