#
#---resign.i.   'Resign' user from a conference.
#
#   inc(1)  conference number.
#   inc(2)  userid.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 09/19/05 New file.
#: CR 10/25/05 Clear confpartic cache.
#: CR 11/21/05 Replace group_update() with group_update_rule().
#: CR 09/22/09 grouprules.grkey is now auto-increment.
#----------------------------------------------------------

#---Clear the confpartic cache.
set confpartic_age 0

#---Remove participation record, clean out any
#   grouprules for this conf specific to this person, and add
#   a grouprule that gives them no access except the ability to
#   rejoin later if they choose.

set q DELETE FROM resps_seen WHERE userid='$inc(2)' \
         AND items_id IN ( \
             SELECT h.items_id FROM conf_has_items h WHERE h.cnum=$inc(1)  \
                AND (SELECT COUNT(*) FROM conf_has_items  \
                      WHERE items_id=h.items_id) = 1)
set h $sql_query_open ($(q))
eval  $sql_query_close($(h))

#---Add optional/bySelf rule that reduces access, but allows for
#   rejoining later.
set access $group_access ($inc(2) CONF $inc(1))
set query DELETE FROM grouprules WHERE owner='CONF' AND name='$inc(1)'  \
             AND userid='$inc(2)' AND access < 100
eval %sql ($(query))
set query INSERT INTO grouprules \
          (grkey, owner, name, userid, access, optional, bySelf)  VALUES \
          (NULL, 'CONF', '$inc(1)', '$inc(2)', '$(access)', 1, 1)
eval %sql ($(query))
eval $group_update_rule ($inc(2) $quote(CONF $inc(1)))

#--------------------------------------------------------------------
