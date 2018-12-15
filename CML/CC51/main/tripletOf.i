#
#---tripletOf.i  Implementation of %tripletOf(cnum word)
#
#:CR 12/05/10 New.
#------------------------------------------------------------

if $equal ($inc(2) last)
   set pq \
       SELECT $inc(1), items_id, 0 FROM conf_has_items  \
        WHERE cnum=$inc(1) AND hidden=0 AND retired=0 AND deleted=0 \
        ORDER BY item0 DESC LIMIT 1
   "$unquote($sql_query_select($(pq)))
end
else
   set _tripletOf $item_parse($inc(1) $inc(2))
   if $greater ($word(1 $(_tripletOf)) 0)
      "$(_tripletOf)
   end
end

