#
#---itemCopy.i   Copy item, or responses to an existing item.
#
# Input:
#   $inc(1)     number of conference (to copy/move to)    [$form(sconf)]
#   $inc(2)     how to copy: new, append, brk             [$form(towhere)]
#   $inc(3)     what item (label) to copy to              [$form(itemno)]
#   $inc(4)     anonymous copy checkbox                   [0$form(anonly)]
#   $inc(5)     list of responses (or "all")              [$quote'd list of response #s]
#   $inc(6)     number of current conference              [icnum]
#   $inc(7)     id of current item                        [iinum]
#   $inc(8)     1=>mark with current date, 0=>use orig date

#eval $append(/tmp/work5 I        srcConf=$inc(6) \
#             tgtConf=$inc(1) srcId=$inc(7) tgtLabel=$inc(3) checklist=$(checklist)$newline())

#
# Output:
#   to_cnum          target conference #
#   newItem          newly created item
#   to_rd            target response.
#   rval             0 success
#                    1 no access to conference
#                    2 no item supplied
#                    3 no such item
#                    4 adding failed (frozen?)
#                    5 cannot create new item
#                    6 copying not allowed
#                    7 no responses supplied in inc(5)
#
# Copyright (C) 1995-2007 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 11/24/07 New file, based on old indv_cop.i
#: CR 07/11/10 Don't copy deleted responses; use highest version #; copy item type.
#-----------------------------------------------------------------------

set  to_cnum   $inc(1)
set  ilabel    $item_label ($inc(6) $inc(7))
set  to_id    -1

#---If not an organizer (in to_cnum), and copying not allowed, fail.
set allowcopy $word (1 $site_data ($(to_cnum) - allowcopy) $(allow_copy))
if $and ($less (%access($(to_cnum)) $priv(instructor)) \
         $not ($(allowcopy)))
   set rval 6
   return 
end

set iname  %displayed_conf_name ($inc(6))

#---If no responses checked, fail!
if $empty ($inc(5))
   set rval 7
   return
end

set rfirst $word (1 $inc(5))
if $equal ($(rfirst) all)
   set rfirst 0
end
set anon   $and  (0$inc(4) \
                  $gt_equal(%access($(to_cnum)) $priv(instructor)))

#---Parse item & response #s for append or breakout.
set to_rd 0
if $not_empty($inc(3))
   include $(main)/itemParse.i $(to_cnum) $quote($inc(3))
   set to_id   $word (2 $(triplet) 0 0 0)
   set to_rnum $word (3 $(triplet) 0 0 0)
end

#---Set up item title and owner if we create a new regular or breakout item.
if $equal ($(rfirst) 0)
   set title %item_title($inc(7))
   set owner $sql_query_select(SELECT userid FROM items WHERE id=$inc(7))
end
else
   set title Discussion from $(iname) item $(ilabel)
   set owner $userid()
end
set type $sql_query_select(SELECT type FROM items WHERE id=$inc(7))

#---New item?
if $equal($inc(2) new)
   set newItem $item_add ($(to_cnum) 0        0          $(owner) $(title))
   if $greater ($(rfirst) 0)
      set newr $resp_add ($(to_cnum) $(newItem) $userid() 0 0 0 \
                    The first $sizeof($inc(5)) responses  \
                    have been copied from $(iname) item $(ilabel).)
      set to_rd $min($(to_rd) $(newr))
   end
end

#---Breakout item?
elif $equal($inc(2) brk)
   set newItem $item_add ($(to_cnum) $(to_id) $(to_rnum) $(owner) $(title))
end

else
   set newItem $(to_id)
end

#---At this point we must have a parsed existing item, new item, or breakout item id.
if $less ($(newItem) 0)
   set rval 5
   return
end

#---Update new item type.
set q2  UPDATE items SET type='$(type)' WHERE id=$(newItem)
set hc2 $sql_query_open ($(q2))
eval    $sql_query_close($(hc2))

#---Copy the selected responses over.
set copyQuery SELECT r.rnum, r.userid, r.text, r.time, r.prop, r.bits, \
                     r.edittime, r.editid, r.versioned, r.approved, \
                     r.author_shows_as  \
                FROM resps r \
               WHERE r.items_id=$inc(7) AND r.deleted=0 AND r.version= \
                 (SELECT MAX(q.version) FROM resps q WHERE q.items_id=r.items_id AND q.rnum=r.rnum)
if $not_equal ($word(1 $inc(5)) all)
   set copyQuery $(copyQuery) AND rnum IN ($commaList(n $inc(5)))
end
set copyQuery $(copyQuery) ORDER BY rnum
set hc $sql_query_open($(copyQuery))
while  $sql_query_row ($(hc))

   set newr  $resp_add ($(to_cnum) $(newItem) $(userid) $(prop) $(bits) 0 $(text))
   set to_rd $min($(to_rd) $(newr))
   if $less ($(newr) 0)
      set rval 4
      return
   end

   #---Copy other properties
   include $(main)/getCurrentVersionOfResponse.i $(newItem) $(newr)
   set q2  UPDATE resps SET edittime='$(edittime)', author_shows_as='$(author_shows_as)', \
                            editid='$(editid)', versioned=$(versioned), approved=$(approved) \
            WHERE items_id=$(newItem) AND rnum=$(newr) AND version=$(currentVersion)
   set hc2 $sql_query_open ($(q2))
   eval    $sql_query_close($(hc2))

   set copyData copy_rnum=$(rnum), copy_item=$inc(7), \
                copy_cnum=$inc(6), copier='$userid()', \
                copy_time=now(), copy_show=$not($inc(4))

   #---Preserve original date/time stamp?
   if $not($inc(8))
      set copyData $(copyData), time='$(time)'
   end

   #---Add all of the 'copied' information.
   set q2  UPDATE resps SET $(copyData) WHERE items_id=$(newItem) AND rnum=$(newr) AND version=$(currentVersion)
   set hc2 $sql_query_open ($(q2))
   eval    $sql_query_close($(hc2))

   #---for each file in old dir, copy2lib to_inum, new resp
   include $(main)/rlibcopy.i $inc(6)   $inc(7) $(rnum) \
                              $(to_cnum) $(newItem) $(newr)
end
eval $sql_query_close($(hc))

#---Update time-last-modified for the conference.  In theory,
#   we should lock this first...
if $greater ($minus ($time() \
              %epoch_of ($site_data ($(to_cnum) - modified))) 300)
   eval $set_site_data  ($(to_cnum) - modified $time())
end

set rval 0
return

#------------------------------------------------------------------
