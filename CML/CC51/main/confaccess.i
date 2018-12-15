#
#---confaccess.i    Check (and promote if necessary) conference access.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/25/05 New file.
#: CR 11/21/05 Replace group_update() with group_update_rule().
#: CR 04/16/07 Use _access as a local variable, don't trash global 'access'!
#: CR 09/22/09 grouprules.grkey is now auto-increment.
#-------------------------------------------------------------------------

set _access $group_access ($userid() CONF $inc(1))

#---If this is a user with "optional" access, they're trying to get in --
#   promote them to their proper level of access.
if $between (1 $(_access) 9)
   set query DELETE FROM grouprules WHERE owner='CONF' AND name='$inc(1)' \
                AND userid='$userid()' AND bySelf=1
   eval %sql ($(query))
   set query \
       INSERT INTO grouprules (grkey, owner, name, userid, access, bySelf) \
       VALUES (NULL, 'CONF', '$inc(1)', '$userid()', $mult($(_access) 10), 1)
   eval %sql($(query))
   eval $group_update_rule ($userid() $quote(CONF $inc(1)))
end

#-------------------------------------------------------------------------
