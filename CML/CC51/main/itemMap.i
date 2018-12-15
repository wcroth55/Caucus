#
#---itemMap.i  Common code for %itemMap() and %itemMapEmbed()
#
#   inc(1)  conf num
#   inc(2)  itemLabels (comma sep)
#   inc(3)  zoom (0=>default)
#   inc(4)  map_canvas index #
#   inc(5)  script tag
#
#:CR 10/14/10 New script.
#---------------------------------------------------------------------

set zoom $if ($equal ($inc(3) 0) 7 $inc(3))

include $(main)/itemParse.i $inc(1) $quote($inc(2))
set items
for ci ii ri in $(triplet)
   set items $(items) $(ii)
end
set items $commaList(number $(items))

set query SELECT r.rnum, r.lat_long, h.item0, h.item1, h.item2, h.item3, h.item4, h.item5, \
                 i.title, i.id \
            FROM resps r, conf_has_items h, items i \
           WHERE h.cnum = $inc(1) \
             AND h.items_id IN ($(items)) AND h.items_id = r.items_id \
             AND h.items_id = i.id \
             AND r.deleted = 0 AND r.version = 0 AND r.rnum = 0

#---Only build this javascript once per page!
if $equal ($inc(4) 1)
   "<$inc(5)
   "   src="http://maps.google.com/maps?file=api&amp;v=2&amp;sensor=false&amp;key=$config(GoogleMapKey)"
   "           type="text/javascript"></$inc(5)>

   "<$inc(5) type="text/javascript">
   "   function createMarker (point, myTitle, itemId) {
   "      var marker = new GMarker(point, { title: myTitle });
   "      GEvent.addListener  (marker, "click", function() {
   "         pop_up('item', '$inc(1)+' + itemId);
   "      });
   "      return marker;
   "   }
   "</$inc(5)>
end

"<$inc(5) type="text/javascript">
"     if (GBrowserIsCompatible()) {
"       var point;
"       var marker;
"       var map = new GMap2(document.getElementById("map_canvas$inc(4)"));
"       map.addControl(new GLargeMapControl());
"       map.addControl(new GMapTypeControl());
   
"       var latSum = 0.;
"       var longSum = 0.;
"       var count = 0;
set h $sql_query_open ($(query))
while $sql_query_row  ($(h))

   set latLongParts $replace(, 32 $(lat_long))
   set latitude  $word (1 $(latLongParts))
   set longitude $word (2 $(latLongParts))
   set latLongParts $replace(.  0 $(latLongParts))
   if $and ($not_empty($(latitude))  $numeric ($word(1 $(latLongParts))) \
            $not_empty($(longitude)) $numeric ($word(2 $(latLongParts))) \
            $equal ($sizeof($(lat_long)) 1)   )
      " latSum  += $(latitude);
      " longSum += $(longitude);
      " count++;

      set label $item_tuplet ($(item0) $(item1) $(item2) $(item3) $(item4) $(item5))
      if $greater ($(rnum) 0)
         set label $(label):$(rnum)
      end
      " point = new GLatLng($(lat_long));
      " marker = createMarker (point, "$escquote($(label) $(title))", "$(id)");
      " map.addOverlay(marker);
   end
end
eval  $sql_query_close($(h))
"       map.setCenter(new GLatLng(latSum/count, longSum/count), $(zoom));
"     }

"</$inc(5)>
