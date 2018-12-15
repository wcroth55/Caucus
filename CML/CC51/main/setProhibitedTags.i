#
#---setProhibitedTags.i
#
#   Input:
#      inc(1)  conf num
#      inc(2)  item id
#      inc(3)  rnum
#      tag_list
#
#   Output:
#      prohibited
#
#   Decide which set of prohibited tags to use, based on whether
#   the author was an organizer.
#----------------------------------------------------

set prohibited $(tag_list)
if $equal($re_bits($inc(1) $inc(2) $inc(3)) 16)
   if $not_equal(x$site_data($inc(1) - tag_list_org) x)
      set prohibited $site_data($inc(1) - tag_list_org)
   end
end
else
   if $not_equal(x$site_data($inc(1) - tag_list_user) x)
      set prohibited $site_data($inc(1) - tag_list_user)
   end
end
