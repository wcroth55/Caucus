#
#---MKITEM.I     Write the item file(s)
#
#   Input:
#      inc(1)   conf num
#      inc(2)   item num
#      inc(4)   next item
#
#   Output:
#      inc(3)   name of variable containing list of last pages.
#               Add the last page # for this item onto
#               the end of the var.
#
#   Assumes conference has been joined, and that the item exists.
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR  7/11/04 Handle displaying "reversed" items.
#==========================================================================

#---Count number of PAGES of (non-deleted) responses; create
#   a response list (RLISTn) for each page.
set pages  1
set rlist0
set rlist1

set reverse 0$site_data ($inc(1) $inc(2) reverse)

set q SELECT i.title, i.lastresp, i.userid, u.fname, u.lname \
        FROM items i \
        LEFT JOIN user_info u \
          ON (u.userid = i.userid) \
       WHERE i.id=$(inum)

#set q SELECT i.title, i.lastresp, u.userid, u.fname, u.lname FROM items i, user_info u \
#       WHERE id=$(inum) AND i.userid = u.userid

set h $sql_query_open ($(q))
eval  $sql_query_row  ($(h))
eval  $sql_query_close($(h))

set q SELECT DISTINCT rnum FROM resps WHERE items_id = $inc(2) AND deleted=0 \
       ORDER BY rnum $if ($(reverse) DESC ASC)
set h $sql_query_open ($(q))
while $sql_query_row  ($(h))
   if $gt_equal ($sizeof($(rlist$(pages))) $(pageresps))
      set pages $plus($(pages) 1)
      set rlist$(pages)
   end
   set rlist$(pages) $(rlist$(pages)) $(rnum)
   set rlist0        $(rlist0)        $(rnum)
end
eval $sql_query_close($(h))

#---Build root for file name.
set fcnum $mac_expand (%digit4($inc(1)))

#---Create individual pages of item.
count page 1 $(pages)
   include $(dir)/showpage.i $(fcnum) $inc(2) $(page) $(pages) rlist$(page) \
                             $inc(4)
end
set   lastpagename  $inc(3)
set $(lastpagename) $($(lastpagename)) $(pages)

#---Create "all" (entire item) page.
include $(dir)/showpage.i $(fcnum) $inc(2) 0 0 rlist0 $inc(4)

#---Text-only version.
if $(dotext)
   include $(dir)/textpage.i $(fcnum) $inc(2) 0 0 rlist0 $inc(4)
end

for udir in $dirlist (0 $caucus_lib()/ITEMS/$inc(2))
   include $(dir)/copydir.i $(udir) $caucus_lib()/ITEMS/$inc(2) \
                                    $(target)/LIB/ITEMS/$inc(2)
end

#==========================================================================
