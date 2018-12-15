#
#---deleteItem.i  Mark this item (and all subitems) as deleted.
#
#   inc(1)  cnum
#   inc(2)  item id
#
# CR 05/01/2010 New file.
#-------------------------------------------------------------------

set visitIds  $inc(2)
set deleteIds $inc(2)

while $not_empty ($(visitIds))
   set query SELECT items_id FROM conf_has_items  \
              WHERE items_parent IN ($commaList(n $(visitIds))) AND cnum=$inc(1) \
                AND deleted = 0
   set visitIds  $sql_query_select($(query))
   set deleteIds $(deleteIds) $(visitIds)
end

set deleteTime $time()
set query UPDATE conf_has_items SET deleted=$(deleteTime) \
           WHERE cnum=$inc(1) AND items_id IN ($commaList(n $(deleteIds)))
set h $sql_query_open ($(query))
eval  $sql_query_close($(h))

