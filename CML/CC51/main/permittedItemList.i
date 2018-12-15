#
#---permittedItemList.i  Filter a list of items to remove those that the user
#      cannot access due to individual item permissions.
#
#   Input: (variables)
#      inc(1)    name of variable containing list of items
#      inc(2)    conference key
#      inc(3)    userid
#
#   Output:
#      permittedItemList       triplet list of permitted items.
#
# Copyright (C) 2008 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 10/10/08 New file.
#-----------------------------------------------------------------------

set permittedItemList $($inc(1))

#---Organizers/instructors always have access.
if $gt_equal ($group_access ($inc(3) CONF $inc(2)) $priv(instructor))
   return
end

#---No change if there are no individually-permitted items.
set restrictedItems $sql_query_select(\
   SELECT name FROM groups WHERE owner='Item' AND name IN ($commaList(string $($inc(1)))))
if $equal ($sizeof($(restrictedItems)) 0)
   return
end

set permittedItemList
for cn id rn in $($inc(1))
   if $tablefind ($(id) $(restrictedItems))
      if $less ($group_access ($inc(3) Item $(id)) $priv(minimum))
         continue
      end
   end
   set permittedItemList $(permittedItemList) $(cn) $(id) $(rn)
end

#-----------------------------------------------------------------------
