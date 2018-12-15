   #---Build triplet list of new itemList.
   set q SELECT i.id FROM (conf_has_items h, items i) \
           LEFT JOIN resps_seen s  \
                  ON (s.userid='$inc(2)' AND s.items_id = i.id) \
          WHERE h.cnum=$inc(1) AND h.items_id = i.id AND s.items_id IS NULL \
            AND h.hidden=0     AND h.retired=0   AND h.deleted = 0 \
          ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
   set hil $sql_query_open ($(q))
   while $sql_query_row  ($(hil))
      set itemList $(itemList) $inc(1) $(id) 0
   end
   eval  $sql_query_close($(hil))
   set itemLabelSome all the new material
   set itemLabelNone <B>There are no items with new discussion.</B>
