#
#---local_bridge.i   Localized interface switching.
#
#   Purpose:
#      The "bridge", in this sense, is a means of having people come into
#      Caucus via a particular URL (the "bridge interface"), add them
#      to a special group, and then "switch" them over to a different
#      interface.
#
#   How it is used:
#      Assume the "target" interface (where the people will end up)
#      is called XYZ.  As a manager, create (from XYZ) a new interface
#      called XYZ_BRIDGE.  Then either:
#         (a) Copy this file to XYZ/main.1/local_iface.i, or
#         (b) Edit local_iface.i to contain the line:
#                include $(main)/local_bridge.i
#
#      Now any users that enter (or register!) via the XYZ_BRIDGE interface
#      will automatically get added to the System group xyz_bridge_members, 
#      and then get "switched" to XYZ.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 03/14/06 New file.
#: CR 09/22/09 grouprules.grkey is now auto-increment.
#-------------------------------------------------------------------

#---Determine the name(s) of the target interface, and the special System group.
set newInterface $word (1 $replace (/ 32 $cml_dir()))
set uscore $str_revdex (_bridge $lower($(newInterface)))
if $greater ($(uscore) 0)
   set newInterface $str_sub (0 $(uscore) $(newInterface))
end
set addGroup $lower($(newInterface))_bridge_members

#---If this user is not a member of addGroup, add them to it.
if $not ($group_access ($userid() Caucus4 $(addGroup)))
  set q1 INSERT INTO grouprules (grkey, owner, name, userid, access) \
         VALUES (NULL, 'Caucus4', '$(addGroup)', '$userid()', $priv(include))
  set h1 $sql_query_open ($(q1))
  eval   $sql_query_close ($(h1))
  eval   $group_update_rule ($userid() $quote(Caucus4 $(addGroup)))
end

#---Switch to the IPY interface.
set iface $(newInterface)
set main  $(newInterface)/main
set help  $(newInterface)/help

