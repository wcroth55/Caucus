#
#---checkItemVis.i   Update items with "visibility by date".
#
#   If we haven't checked item visibility today... do so!
#
#   Check all items in this conf that have a "visibility by date"
#   set.  Set their visibility based on today's date and their
#   visibility rules.
#
#   inc(1)  conf num
#
#: CR 12/20/2010
#------------------------------------------------------------------------------------

if $empty ($(checkItemVis))
   set checkItemVis true

   set lastDateChecked $word (1 $dateof(0$site_data ($inc(1) - lastDateItemVisChecked)))
   set thisDateChecked $word (1 $dateof($time()))
   if $not_equal ($(lastDateChecked) $(thisDateChecked))
      eval $set_site_data ($inc(1) - lastDateItemVisChecked $(thisDateChecked))
   
      set visQuery SELECT inum FROM site_data \
                    WHERE cnum=$inc(1) AND name IN ('visdate0', 'visdate1') AND inum!='-'
      set vh $sql_query_open ($(visQuery))
      while  $sql_query_row  ($(vh))
         set visibleAfter  $word (1 $site_data ($inc(1) $(inum) visdate0) 0)
         set visibleBefore $word (1 $site_data ($inc(1) $(inum) visdate1) $plus($time() 86400))
         set visibleBefore $plus ($(visibleBefore) 86400)
         set isHidden $not($between ($(visibleAfter) $time() $(visibleBefore)))
         eval $item_set ($inc(1) $(inum) hidden $(isHidden))
      end
      eval   $sql_query_row  ($(vh))
   end
end
