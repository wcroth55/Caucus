#
#---readToBeApprovedItems
#
#   inc(1) cnum

set apQuery \
 SELECT i.id \
      FROM items i, resps r, conf_has_items h \
     WHERE i.type = 'modblog'  \
       AND i.id = h.items_id AND h.cnum=$inc(1) \
       AND i.id = r.items_id AND r.rnum=0 \
       AND r.version =  \
          (SELECT MAX(q.version) FROM resps q WHERE q.items_id=i.id AND rnum=0) \
       AND h.item1 != 0 AND h.item2 = 0 \
       AND r.approved = 0

set approveTriplet
set h $sql_query_open ($(apQuery))
while $sql_query_row  ($(h))
   set approveTriplet $(approveTriplet) $inc(1) $(id) 0
end
eval  $sql_query_close($(h))

set approveCount $divide($sizeof($(approveTriplet)) 3)
if $greater ($(approveCount) 0)
   set in $word(2 $(approveTriplet))
   "There are <b><a href="viewitem.cml?$(nch)+$arg(2)+$inc(1)+$(in)+0+x+1+approveTriplet#here"
   "   >$(approveCount) new or edited blog entries</a></b>
   "that need your approval.
end
