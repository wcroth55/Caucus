#
#---groupadd.i   Add a userid to a group.
#
#   inc(1)  group owner (type)
#   inc(2)  group name
#   inc(3)  userid to add
#   inc(4)  access
#
#: CR 04/02/06 New file.
#: CR 09/22/09 grouprules.grkey is now auto-increment.
#----------------------------------------------------------

set _q INSERT INTO grouprules \
   (grkey, owner, name, userid, access)  VALUES \
   (NULL, '$inc(1)', '$inc(2)', '$inc(3)', $inc(4))
set _h $sql_query_open ($(_q))
eval   $sql_query_close($(_h))
eval $group_update_rule ($inc(3) $quote ($inc(1) $inc(2)))
