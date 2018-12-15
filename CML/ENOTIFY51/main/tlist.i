#
#---TLIST.I    Build a triplet list of items from an item group description.
#
#   Invoked as:
#      tlist.i  tlistname   item_group_var
#
#   Input variables:
#      inc(1)    name of tlist variable to put triplet list in
#      inc(2)    name of variable containing item group
#      cnum      conference number
#
#   Purpose:
#      Given an e-notify item group description, build the
#      equivalent triplet list of items for that group.
#      Set the response number of each triplet to the last
#      undeleted response for that item.
#
#   How it works:
#
#   Known bugs:
#
#   History of revisions:
#: CR  3/07/99 16:16 First version.
#: CR  6/02/99 13:29 Corrected to remove duplicates; better comments
#----------------------------------------------------------------------------

#---Parse the list of items; add on newly created items if requested.

#---Build the initial list of items.
set en_items $($inc(2))
set en_list  $rest (6 $(en_items))
include $(main)/itemParse.i $(cnum) $quote($(en_list))

if $word (2 $(en_items))
   set highest $word (4 $(en_items))

   #---Pre 5.1 style: highest item number known.  Convert it to MySQL time.
   if $less ($str_index(- $(highest)) 0)
      set highest $quote($sql_query_select (SELECT time FROM resps \
             WHERE items_id=$item_id($(cnum) $(highest)) AND rnum=0 AND version=0))
   end

   #---Post 5.1 style: find all items created at or after epoch time.
   set pq SELECT $(cnum), h.items_id, 0 FROM conf_has_items h, resps r \
           WHERE h.cnum=$(cnum) and h.items_id=r.items_id AND r.rnum=0 \
             AND r.time > '$unquote($(highest))' AND r.version=0
   set triplet $(triplet) $unquote($sql_query_select($(pq)))
end

#---Modify response part of triplet list to contain last response on each item
set trip2
for ic ii ir in $(triplet)
   set rlast %item_lastresp ($(ii))
   if $gt_equal ($(rlast) 0)
      set trip2 $(trip2) $(ic) $(ii) $(rlast)
   end
end

set $inc(1) $(trip2)

#----------------------------------------------------------------------------
