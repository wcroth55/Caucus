#
#---group_addnewusers.i   Add newly created users to 'groups' table.
#
#   inc(1)   new userid
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 09/19/05 New file.
#: CR 11/21/05 Do all work in current process (not sweba) via
#              group_update_rule().
#--------------------------------------------------------------------

#---Make sure new user is in interface storage conf so they can see 
#   the interface!
eval $group_update_rule ($inc(1) \
                         $quote(CONF $cl_num(interface_storage_$(iface))))

#---Find all groups with wildcard grouprules that match this userid.
set query SELECT DISTINCT owner, name FROM grouprules \
           WHERE wildcard=1 AND '$inc(1)' LIKE userid

set h $sql_query_open ($(query))
while $sql_query_row  ($(h))
   eval $group_update_rule ($inc(1) $quote($(owner) $(name)))
end
eval  $sql_query_close($(h))

#--------------------------------------------------------------------

