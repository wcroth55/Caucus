#
#--- computeItemAccess.i   Determine complete access levels to this item.
#
#    Assumes that we are working with an item somehow -- viewing it, editing it, etc.
#
#    Input:
#       inc(1)  conference number
#       inc(2)  item id
#
#    Output:
#       itemAccess  access level to this item
#       is_org      user has organizer  access to this conference
#       is_ins      user has instructor access to this conference
#       is_incl     user has include    access to this conference
#
#:CR 08/19/09 New include file.
#:CR 08/31/09 Handle returns from $group_access() < 0.
#:CR 03/03/10 Properly handle items with individual access.
#---------------------------------------------------------------------------------

set confAccess %access($inc(1))
set is_org     $gt_equal ($(confAccess) $priv(organizer))
set is_ins     $gt_equal ($(confAccess) $priv(instructor))
set is_incl    $gt_equal ($(confAccess) $priv(include))

set itemAccess $(confAccess)
if $not ($(is_org))
   set tmpAccess $group_access ($userid() Item $inc(2))
   if $greater ($(tmpAccess) 0)
      set itemAccess $(tmpAccess)
   end
   else
      set restrictedItems $sql_query_select(\
          SELECT name FROM groups WHERE owner='Item' AND name=$inc(2))
      if $greater ($sizeof($(restrictedItems)) 0)
         set itemAccess 0
      end
   end
end

