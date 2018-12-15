#
#---itemParse.i.   Parse an item label or item title to a triplet list.
#
#   Input:
#      inc(1)  cnum
#      inc(2)  item range string (often quoted)
#
#   Output:
#      triplet
#
#   Parses a variety of item ranges.  Each "range" is separated by
#   commas.   Each "range" can be:
#      an item label (with an optional :response number)
#      a range of item labels, e.g. 1-5 (but not, say, 1.1-5)
#      the special word "all", which means all items
#      part of an item title
#
#   If an individual element does not match anything, it is skipped.
#
#   Ensures that each item in the triplet list is visible
#   (not forgotten or hidden).
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 03/01/2007 New file.
#:CR 03/13/2007 Rewrite to handle wider variety of ranges.
#:CR 12/07/2010 Handle special cases like "5-last".
#------------------------------------------------------------------

set _triplet
set allRange $replace(, 32 $quote($inc(2)))
for block in $(allRange)
   set range $unquote($(block))
   if $equal ($lower($(range)) all)
      set pq \
          SELECT $inc(1), items_id, 0 FROM conf_has_items  \
           WHERE cnum=$inc(1) AND hidden=0 AND retired=0 \
           ORDER BY item0, item1, item2, item3, item4, item5
      set _triplet $(_triplet) $unquote($sql_query_select($(pq)))
   end
   elif $greater ($is_num($(range)) 0)
      for word in $(range)
         if $greater ($str_index(- $(word)) 0)
            set words $replace (- 32 $(word))
            set w1    $item_label($inc(1) $word (2 %tripletOf($inc(1) $word (1 $(words)))))
            set w2    $item_label($inc(1) $word (2 %tripletOf($inc(1) $word (2 $(words)))))
            set pq \
                SELECT $inc(1), items_id, 0 FROM conf_has_items  \
                 WHERE cnum=$inc(1) AND item0 >= $(w1) AND item0 <= $(w2) \
                   AND hidden=0 AND retired=0 \
                 ORDER BY item0, item1, item2, item3, item4, item5
            set _triplet $(_triplet) $unquote($sql_query_select($(pq)))
         end
         else
            set _result $item_parse ($inc(1) $(word))
            if $greater ($word(1 $(_result)) 0)
               set _triplet $(_triplet) $(_result)
            end
         end
      end
   end
   else
      set _triplet $(_triplet) %tripletOf($inc(1) $(range))
   end
end

set  triplet
for c i r in $(_triplet)
   #---This is begging to become $item_visible()!
   set pQuery \
       SELECT h.hidden, s.forgot \
         FROM conf_has_items h \
         LEFT JOIN resps_seen s \
           ON (h.items_id = s.items_id AND h.cnum=$(c) AND s.userid='$userid()') \
        WHERE h.items_id=$(i)
   set hq $sql_query_open ($(pQuery))
   eval   $sql_query_row  ($(hq))
   eval   $sql_query_close($(hq))

   if $or ($(hidden) 0$(forgot))
      continue
   end

   set triplet $(triplet) $(c) $(i) $(r)
end
