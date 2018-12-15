#
#---moveItemToExistingItem.i
#
#   Moves item and all its (breakout) children to be
#   a breakout of another (already existing) item.
#
#   Input
#      inc(1) source cnum
#      inc(2) source item label
#      inc(3) target cnum
#      inc(4) target item label
#
#   Output
#      newItemLabel  resulting item label, or < 0 on error
#
# CR: 4/24/2010 New file
#---------------------------------------------------------------------

set sourceCnum  $inc(1)
set sourceLabel $inc(2)
set sourceId    $item_id($(sourceCnum) $(sourceLabel))
set targetCnum  $inc(3)
set targetLabel $inc(4)
set targetId    $item_id($(targetCnum) $(targetLabel))

#---Make sure target exists!
if $less ($(targetId) 0)
   set newItemLabel -2
   return
end

#---Make sure the resulting depth of the new label isn't too big!
set sourceTuplet $replace(. 32 $(sourceLabel))
set targetTuplet $replace(. 32 $(targetLabel))
if $greater ($sizeof($(sourceTuplet) $(targetTuplet)) 6)
   set newItemLabel -1
   return
end

#---Get new item label 
set itemIndex item$sizeof($(targetTuplet))
include $(main)/itemClauseForLabel.i $(targetLabel)
set query SELECT MAX($(itemIndex)) FROM conf_has_items WHERE cnum=$(targetCnum) \
             AND $(itemClause)
set newTuplet    $(targetTuplet) $plus(1 $word(1 $sql_query_select($(query)) 0))
set newItemLabel $item_tuplet($(newTuplet))
set newTuplet    $str_replace(. $quote(, ) $(newItemLabel))

#---Build query to get item and all of its breakouts.
include $(main)/itemClauseForLabel.i $(sourceLabel)
set queryAllChildItems \
    SELECT items_id, item0, item1, item2, item3, item4, item5, items_parent, resps_parent \
      FROM conf_has_items \
     WHERE cnum=$(sourceCnum) AND $(itemClause)

set startWithWord $plus (1 $sizeof($(sourceTuplet)))

#---For item and its breakouts, insert matching new conf_has_items row for
#   "new" targetLabel item.
set hAll $sql_query_open ($(queryAllChildItems))
while    $sql_query_row  ($(hAll))
   set insertQuery \
       INSERT INTO conf_has_items \
          (cnum, items_id, item0, item1, item2, item3, item4, item5, items_parent, resps_parent) \
       VALUES ($(targetCnum), $(items_id), \
          $words(1 6 $(newTuplet), \
                 $words ($(startWithWord) 6 $(item0), $(item1), $(item2), $(item3), $(item4), $(item5),)) \
          $(items_parent), $(resps_parent))
   set h $sql_query_open ($(insertQuery))
   eval  $sql_query_close($(h))
end
eval     $sql_query_close($(hAll))


#---Fix up the item itself, to repoint to its new parent.
set fixQuery \
    UPDATE conf_has_items SET items_parent=$(targetId), resps_parent=0 \
     WHERE cnum=$(targetCnum) AND items_id=$(sourceId)
set h $sql_query_open ($(fixQuery))
eval  $sql_query_close($(h))

#---Finally, delete the old rows.
set deleteOldEntries \
    DELETE FROM conf_has_items WHERE cnum=$(sourceCnum) AND $(itemClause)
set h $sql_query_open ($(deleteOldEntries))
eval  $sql_query_close($(h))
