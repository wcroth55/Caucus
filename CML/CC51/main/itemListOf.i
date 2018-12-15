#
#---itemListOf.i   Translate item selection into triplet list of items.
#
#   Input: (variables)
#      inc(1)    conference key
#      inc(2)    userid
#      inc(3)    selection type (itype: all, new, inew, rnew, num, etc. -- see iselect.i)
#      inc(4)    1=>include forgotten
#      inc(5)    1=>include hidden
#      inc(6)    1=>include retired
#      inc(7)    selection data (needed for keys name, num, author, etc.)
#
#   Output:
#      itemList       triplet list of itemList selected
#      itemLabelSome  if not empty: what to call them
#      itemLabelNone  if empty:     what to call them
#
# Copyright (C) 2006 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 06/07/06 Major rewrite of old iresolve.i
#:CR 03/20/08 Default optional args 4,5,6 to 0!
#:CR 10/10/08 Add handling for individual item perms.
#-----------------------------------------------------------------------

set itemList
set itemLabelSome
set itemLabelNone

set i4 $word(1 $inc(4) 0)
set i5 $word(1 $inc(5) 0)
set i6 $word(1 $inc(6) 0)
set andForgotten  $ifthen ($equal ($(i4) 0) AND \
                                    (s.forgot IS NULL  OR s.forgot=0))
set andHidden     $ifthen ($equal ($(i5) 0) AND h.hidden =0)
set andRetired    $ifthen ($equal ($(i6) 0) AND h.retired=0)

if $equal ($inc(3) new)
   include $(main)/itemListOfNewResp.i $inc(1) $inc(2)
   
   include $(main)/itemListOfNewItem.i $inc(1) $inc(2)
end
 
elif $equal ($inc(3) rnew)
   include $(main)/itemListOfNewResp.i $inc(1) $inc(2)
end
 
elif $equal ($inc(3) inew)
   include $(main)/itemListOfNewItem.i $inc(1) $inc(2)
end

elif $equal ($inc(3) all)
   set q SELECT i.id FROM (conf_has_items h, items i) \
           LEFT JOIN resps_seen s  \
             ON (s.userid='$inc(2)' AND s.items_id = i.id) \
          WHERE h.cnum=$inc(1)      AND h.items_id = i.id  \
            $(andHidden)  $(andRetired)  $(andForgotten) \
            AND h.deleted = 0 \
         ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
   set hil $sql_query_open ($(q))
   while $sql_query_row  ($(hil))
      set itemList $(itemList) $inc(1) $(id) 0
   end
   eval  $sql_query_close($(hil))
   set itemLabelSome all the items
   set itemLabelNone <B>There are no items in this conference.</B>
end

elif $equal ($inc(3) mine)
   set q SELECT i.id FROM (conf_has_items h, items i) \
           LEFT JOIN resps_seen s  \
             ON (s.userid='$inc(2)' AND s.items_id = i.id) \
          WHERE h.cnum=$inc(1)        AND h.items_id = i.id  \
            $(andHidden)  $(andRetired)  $(andForgotten) \
            AND h.deleted = 0 \
            AND i.userid='$inc(2)' \
         ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
   set hil $sql_query_open ($(q))
   while $sql_query_row  ($(hil))
      set itemList $(itemList) $inc(1) $(id) 0
   end
   eval  $sql_query_close($(hil))
   set itemLabelSome all my items
   set itemLabelNone <B>You have no items in this conference.</B>
end

elif $equal ($inc(3) author)
   set uids $commaList(string $peo_names (0 $(idata)))
   set q SELECT i.id FROM (conf_has_items h, items i) \
           LEFT JOIN resps_seen s  \
             ON (s.userid='$inc(2)' AND s.items_id = i.id) \
          WHERE h.cnum=$inc(1)      AND h.items_id = i.id  \
            $(andHidden)  $(andRetired)  $(andForgotten) \
            AND i.userid IN ($(uids)) \
            AND h.deleted = 0 \
         ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
   set hil $sql_query_open ($(q))
   while $sql_query_row  ($(hil))
      set itemList $(itemList) $inc(1) $(id) 0
   end
   eval  $sql_query_close($(hil))

   set itemLabelSome items by '$(idata)'
   set itemLabelNone <B>There are no items by '$(idata)'</B>.
end

elif $equal ($inc(3) since)
   set q SELECT i.id FROM (conf_has_items h, resps r, items i) \
           LEFT JOIN resps_seen s  \
             ON (s.userid='$inc(2)' AND s.items_id = i.id) \
          WHERE h.cnum=$inc(1)      AND h.items_id = i.id  \
            $(andHidden)  $(andRetired)  $(andForgotten) \
            AND i.id = r.items_id   AND r.rnum = i.lastresp  AND r.version= \
              (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum) \
            AND r.time >= '%yyyymmddOf($(idata))' \
            AND h.deleted = 0 \
         ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
   set hil $sql_query_open ($(q))
   while $sql_query_row  ($(hil))
      set itemList $(itemList) $inc(1) $(id) 0
   end
   eval  $sql_query_close($(hil))

   set itemLabelSome items active since $(idata)
   set itemLabelNone <B>There are no items with activity since $(idata)</B>.
end

elif $equal ($inc(3) past)
   set q SELECT i.id FROM (conf_has_items h, resps r, items i) \
           LEFT JOIN resps_seen s  \
             ON (s.userid='$inc(2)' AND s.items_id = i.id) \
          WHERE h.cnum=$inc(1)      AND h.items_id = i.id  \
            $(andHidden)  $(andRetired)  $(andForgotten) \
            AND i.id = r.items_id   AND r.rnum = i.lastresp AND r.version= \
              (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum) \
            AND r.time >= date_sub(now(), interval $(idata) day) \
            AND h.deleted = 0 \
         ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
   set hil $sql_query_open ($(q))
   while $sql_query_row  ($(hil))
      set itemList $(itemList) $inc(1) $(id) 0
   end
   eval  $sql_query_close($(hil))

   set itemLabelSome items active in past $(idata) days
   set itemLabelNone <B>There are no items with activity in past $(idata) days</B>.
end
 
elif $equal ($inc(3) forgot)
   set q SELECT i.id FROM (conf_has_items h, items i) \
           LEFT JOIN resps_seen s  \
             ON (s.userid='$inc(2)' AND s.items_id = i.id) \
          WHERE h.cnum=$inc(1)      AND h.items_id = i.id  \
            $(andHidden)  $(andRetired)  AND s.forgot=1 \
            AND h.deleted = 0 \
         ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
   set hil $sql_query_open ($(q))
   while $sql_query_row  ($(hil))
      set itemList $(itemList) $inc(1) $(id) 0
   end
   eval  $sql_query_close($(hil))
   set itemLabelSome forgotten items
   set itemLabelNone <B>There are no forgotten items.</B>
end

elif $equal ($inc(3) none)
   set itemList
   set itemLabelSome
   set itemLabelNone
end

elif $equal ($inc(3) num)

   #---Filter item number list into parseable format.
   set idata $replace(, 32 $(idata))
   while $greater ($str_index ($quote(- ) $quote($(idata))) -1)
      set idata  $str_replace ($quote(- ) - $(idata))
   end
   while $greater ($str_index ($quote( -) $quote($(idata))) -1)
      set idata  $str_replace ($quote( -) - $(idata))
   end

   #---Turn parseable list into AND/OR conditions to match item label numbers.
   set matchItems
   for token in $(idata)
      if $gt_equal ($str_index(- $(token)) 0)
         set token $replace(- 32 $(token))
         set clause h.item0>=$word(1 $(token)) AND h.item0<=$word(2 $(token))
      end
      else
         set count 0
         set clause
         for number in $replace(. 32 $(token))
            set clause $(clause) h.item$(count)=$(number)
            set count $plus($(count) 1)
         end
         set clause $str_replace (, $quote( AND ) $commaList(number $(clause)))
      end

      if $empty ($(matchItems))
         set matchItems ($(clause))
      end
      else
         set matchItems $(matchItems) OR ($(clause))
      end
   end

   set q SELECT i.id FROM (conf_has_items h, items i) \
           LEFT JOIN resps_seen s  \
             ON (s.userid='$inc(2)' AND s.items_id = i.id) \
          WHERE h.cnum=$inc(1)      AND h.items_id = i.id  \
            $(andHidden)  $(andRetired)  $(andForgotten) \
            AND h.deleted = 0 \
            AND ($(matchItems)) \
         ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
   set hil $sql_query_open ($(q))
   while $sql_query_row  ($(hil))
      set itemList $(itemList) $inc(1) $(id) 0
   end
   eval  $sql_query_close($(hil))

   set itemLabelSome the requested items
   set itemLabelNone <B>No items matched '$(idata)'.</B>
end

elif $equal ($inc(3) name)
   set likeList i.title RLIKE '[[:<:]]$word(1 $(idata))'
   for token in $rest (2 $(idata))
      set likeList $(likeList) OR i.title RLIKE '[[:<:]]$(token)'
   end

   set q SELECT i.id FROM (conf_has_items h, items i) \
           LEFT JOIN resps_seen s  \
             ON (s.userid='$inc(2)' AND s.items_id = i.id) \
          WHERE h.cnum=$inc(1)      AND h.items_id = i.id  \
            $(andHidden)  $(andRetired)  $(andForgotten) \
            AND h.deleted = 0 \
            AND ($(likeList)) \
         ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
   set hil $sql_query_open ($(q))
   while $sql_query_row  ($(hil))
      set itemList $(itemList) $inc(1) $(id) 0
   end
   eval  $sql_query_close($(hil))

   set itemLabelSome the requested itemList
   set itemLabelNone <B>No itemList matched '$(idata)'.</B>
end

#---Filter out any items that have individual item permissions
#   that disallow access to this user.
include $(main)/permittedItemList.i itemList $inc(1) $inc(2)
set itemList $(permittedItemList)

#-----------------------------------------------------------------------
