   #---Build triplet list of items with new responses.
   set q SELECT i.id, s.seen \
           FROM items i, conf_has_items h, resps_seen s \
          WHERE h.cnum=$inc(1)      AND h.items_id = i.id  \
            $(andHidden)            $(andRetired) \
            AND h.deleted = 0 \
            AND s.items_id = i.id   AND s.userid='$inc(2)' \
            AND i.lastresp > s.seen $(andForgotten) \
         ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
   set hil $sql_query_open ($(q))
   while $sql_query_row  ($(hil))
      set itemList $(itemList) $inc(1) $(id) $plus ($(seen) 1)
   end
   eval  $sql_query_close($(hil))
