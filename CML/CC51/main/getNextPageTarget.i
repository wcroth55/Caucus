#
#---getNextPageTarget.i   Figure target and target_name of "next" page
#                         in a list of items.
#
#   Input:
#      vi_cnum   current conf
#      Usual arg(3)...arg(8)
#
#   Output:
#      target_name   text name of target
#
#:CR 04/22/2007 New script.
#-------------------------------------------------------------------------

#---So first we have to determine the TARGET_NAME -- the description of where
#   the button will take us.  It should be one of:
#    "next item"        (if there are still items in the triplet list)
#     CML page index "hidden" at the end of the triplet list
#     original caller of viewitem.cml

if $equal ($arg(7) x)
   set target_name $rest(4 $page_get($arg(2)))
   return
end

#---Look for the next item in the triplet list.
set vi_next     $plus ($arg(7) 3)
set vi_temp     $rest ($(vi_next) $($arg(8)) )
set vi_nextconf $word (1 $(vi_temp))
if $empty($(vi_nextconf))
   set vi_nextconf 0
end

if $equal ($(vi_cnum) $(vi_nextconf))
   set target_name next item
end
else
   set target_name <b>%displayed_conf_name ($(vi_nextconf))</b>
end


#---If there aren't any more, get an index for the page to go to  (TARGET),
#   and its description.
set target
if $empty ($word (2 $(vi_temp)) )
   set target      $if ($(vi_nextconf) $(vi_nextconf) $arg(2))
   set target_name $rest (4 $page_get ($(target)))
end
if $empty ($(target_name))
   set target_name $(center_name)
end

