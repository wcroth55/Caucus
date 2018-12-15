#
#---MKPEOPLE.I     Make all people-related pages.
#
#   Input:
#      inc(1)   confnum
#
#   Output:
#      allusers   adds userids not already in variable 'allusers', to
#                 the end of 'allusers'.
#
#   Purpose:
#
#   Usage:
#
#   How it works
#
#   Notes:
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#==========================================================================

set vi_cnum $inc(1)

#---Get pretty form of conference name.
set cname %displayed_conf_name ($(vi_cnum))

if $less ($group_access ($userid() CONF $(vi_cnum)) $priv(minimum))
   "Cannot join conference $(cname).
   return
end


#---Build root for file name.
set fcnum $mac_expand (%digit4($(vi_cnum)))

set last x
for uid in $peo_members($(vi_cnum))
    if $not_equal ($(uid) $(last))
       set last $(uid)
       include $(dir)/person.i $(uid)
       if $not ($tablefind ($(uid) $(allusers)))
          set allusers $(allusers) $(uid)
       end
    end
end

#---Build "all members" page.
include $(dir)/members.i $(fcnum)

#==========================================================================
