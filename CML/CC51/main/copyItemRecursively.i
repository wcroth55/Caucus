#
#---copyItemRecursively.i
#
#   inc1 srcCnum
#   inc2 targetCnum
#   inc3 srcItem id
#   inc4 targetItem id
#   inc5 copy "anonymously"?  (1=true, 0=false)
#   inc6 mark copy with current date? (1=true, 0=false)
#   inc7 quote'd title for new top item (optional, quizzes only)
#
#:CR 01/24/2011 New.
#-----------------------------------------------------------------------

#eval $append (/tmp/work5 recurse: srcConf=$inc(1) tgtConf=$inc(2) srcId=$inc(3) tgtId=$inc(4)$newline())
set sourceCnum  $inc(1)
set targetCnum  $inc(2)
set targetTitle $inc(7)

#---Check all the ancestors of targetItem (aka node).
#   If any of them are srcItem, abort!
set node $plus(0 $inc(4))
while $greater ($(node) 0)
   set parent $unquote($sql_query_select (\
      SELECT items_parent FROM conf_has_items \
       WHERE cnum=$(targetCnum) AND items_id=$(node) AND deleted=0))
   if $equal ($(parent) $inc(3))
      set rval 8
      return
   end
   set node $plus(0 $(parent))
end

eval %clear(sourceAndTargetItemStack)
eval %push (sourceAndTargetItemStack $inc(3):$inc(4))

while $not_empty (%top(sourceAndTargetItemStack))
   set sourceAndTarget $replace(: 32 %pop(sourceAndTargetItemStack))
   set sourceItem $word(1 $(sourceAndTarget))
   set targetItem $word(2 $(sourceAndTarget))

   set itemType $sql_query_select (SELECT type FROM items WHERE id=$(sourceItem))
   if $tablematch ($(itemType) quiz survey)
      set title $(targetTitle)
      set targetTitle
      if $empty($(title))
         set title $sql_query_select (SELECT title FROM items WHERE id=$(sourceItem))
      end

      if $equal($(targetItem) 0)
         include $(main)/quiz_copy.i $(sourceItem) $(targetCnum) $quote($(title)) new
      end
      else
         include $(main)/quiz_copy.i $(sourceItem) $(targetCnum) $quote($(title)) breakout \
            $item_label($(targetCnum) $(targetItem))
      end
      set rval 0
   end 
   else
      if $equal($(targetItem) 0)
         include $(main)/itemCopy.i $(targetCnum) new x \
                                    $inc(5) all $(sourceCnum) $(sourceItem) $inc(6)
      end
      else
         include $(main)/itemCopy.i $(targetCnum) brk $item_label($(targetCnum) $(targetItem)) \
                                    $inc(5) all $(sourceCnum) $(sourceItem) $inc(6)
      end
   end

   if $equal ($(rval) 0)
      set childItems %itemsUnder($(sourceCnum) $(sourceItem))
      for childItem in $(childItems)
         eval %push(sourceAndTargetItemStack $(childItem):$(newItem))
      end
   end
end

