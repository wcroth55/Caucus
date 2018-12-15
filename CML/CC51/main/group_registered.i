#
#---group_registered.i    Record user registration via interface to a group.
#
#   inc(1)  userid
#   iface   interface in question
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 09/19/05 New file.
#: CR 11/21/05 Replace group_update() with group_update_rule().
#: CR 09/22/09 grouprules.grkey is now auto-increment.
#---------------------------------------------------------------

#---Record the fact that this user was registered via this interface,
#   by placing them in a specially-named group.
set query INSERT INTO grouprules (grkey, owner, name, userid, access) \
    VALUES (NULL, 'Caucus4', 'interface_registered_$(iface)', '$inc(1)', $priv(include))
eval %sql($(query))
eval $group_update_rule ($inc(1) $quote(Caucus4 interface_registered_$(iface)))
