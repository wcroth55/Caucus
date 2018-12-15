#
#---linkItemToNewItem.i
#
#   Create all of the conf_has_item entries to link an
#   item (and any/all breakout sub-items) from a source
#   conference, to a completely new, top-level item in
#   a target conference.
#
#   Input
#      inc(1) source cnum
#      inc(2) source item label
#      inc(3) target cnum
#
#   Output
#      itemClause
#      targetLabel (label of newly "created" item link in target conf)
#
# CR: 4/22/2010 New file
# CR: 3/17/2013 Allow inserting new entry if moving in same conference!
#---------------------------------------------------------------------

set sourceCnum  $inc(1)
set sourceLabel $inc(2)
set sourceId    $item_id($(sourceCnum) $(sourceLabel))
set targetCnum  $inc(3)

#---Get new item number (targetLabel)
set query SELECT MAX(item0) FROM conf_has_items WHERE cnum=$(targetCnum)
set targetLabel $plus (1 $word(1 $sql_query_select($(query))) 0)

#---Build query to get item and all of its breakouts.
include $(main)/itemClauseForLabel.i $(sourceLabel)
set queryAllChildItems \
    SELECT items_id, item0, item1, item2, item3, item4, item5, items_parent, resps_parent \
      FROM conf_has_items \
     WHERE cnum=$(sourceCnum) AND $(itemClause) AND deleted=0

set startWithWord $plus(1 $sizeof($replace(. 32 $(sourceLabel))))

#---For item and its breakouts, insert matching new conf_has_items row for
#   "new" targetLabel item.
set hAll $sql_query_open ($(queryAllChildItems))
while    $sql_query_row  ($(hAll))
   set existingLinks $sql_query_select(\
       SELECT COUNT(*) FROM conf_has_items \
        WHERE cnum=$(targetCnum) AND items_id=$(items_id) AND deleted=0)
   set insertNewEntry $or ($equal ($(existingLinks) 0) \
                           $and ($equal ($(existingLinks) 1) $equal($(sourceCnum) $(targetCnum))))
   if $(insertNewEntry)
      set insertQuery \
          INSERT INTO conf_has_items \
             (cnum, items_id, item0, item1, item2, item3, item4, item5, \
              items_parent, resps_parent) \
          VALUES ($(targetCnum), $(items_id), $(targetLabel), \
          $words ($(startWithWord) 5 $(item0), $(item1), $(item2), $(item3), $(item4), $(item5), \
                  0, 0, 0, 0, 0,) $(items_parent), $(resps_parent))
      set h $sql_query_open ($(insertQuery))
      eval  $sql_query_close($(h))
   end
end
eval     $sql_query_close($(hAll))


#---Fix up the item itself, since it no longer has parents.
set fixQuery \
    UPDATE conf_has_items SET items_parent=0, resps_parent=0 \
     WHERE cnum=$(targetCnum) AND items_id=$(sourceId)
set h $sql_query_open ($(fixQuery))
eval  $sql_query_close($(h))
