#
#---confpartic.i    Initialize Conference participation for a user.
#
#   Input
#      inc(1)   conference number
#
#   Notes:
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 09/13/05 New file.
#:CR 10/25/05 Add caching.
#---------------------------------------------------------------------

#---If the cache of confs-that-we-have-checked-membership-for is too old,
#   empty it.
if $greater ($minus($time() 0$(confpartic_age)) 1200)
   set confpartic_cache
   set confpartic_age $time()
end

#---If this conf is already cached, we're done.
if $tablefind ($inc(1) $(confpartic_cache))
   return
end

#---Add this conf to cache.
set confpartic_cache $(confpartic_cache) $(cnum)

#---If there is no participation record...
set cpQuery SELECT h.items_id FROM conf_has_items h, resps_seen s \
             WHERE h.cnum=$(cnum) AND h.items_id=s.items_id AND s.userid='$userid()' \
             LIMIT 1
if $empty ($sql_query_select($(cpQuery)))

   #---If organizer set "mark all items as seen for new members", do
   #   so for this member.
   if 0$site_data ($inc(1) - markseen)
      set cqQuery SELECT i.id, i.lastresp, h.hidden FROM items i, conf_has_items h \
                   WHERE h.cnum=$(cnum) AND i.id = h.items_id AND h.hidden=0
      set hq $sql_query_open ($(cqQuery))
      while  $sql_query_row  ($(hq))
         eval $item_set_seen ($(cnum) $(id) $userid() $(lastresp))
      end
      eval $sql_query_close($(hq))
   end
end


