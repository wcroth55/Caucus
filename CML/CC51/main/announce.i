#
#---announce.i   handle displaying announcements
#
#   Notes:
#      2. wipe ann's that are expired?
#
#   'Global' variables
#      ann_seen             list of announcements seen *this session* by 
#                                this user.
#      last_announce_check  epoch time of last announcement check
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 05/16/03 CR New file.
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#: CR 10/10/05 C5 changes.
#: CR 10/25/05 Make pop-over (DHTML) rather than pop-up.
#: CR 10/18/07 Correct for 5.1.
#: CR 11/20/07 Fix incorrect %mgr_in_group() call.
#-------------------------------------------------------------

#---Test for announcements every 10 minutes.
if $less ($time() $plus (0$(last_announce_check) 600))
   return
end
set last_announce_check $time()

#---Get system_announcements cnum.
set ann_cnum $word (1 $cl_list (system_announcements))
if $empty ($(ann_cnum))
   return
end

#---Make sure this user can read it.
include $(main)/confaccess.i $(ann_cnum)
if $not (%access($(ann_cnum)))
   return
end

#---Scan all the non-deleted items in system_announcements:
set aQuery SELECT h.items_id FROM conf_has_items h \
            WHERE h.cnum=$(ann_cnum) AND h.hidden=0 AND h.retired=0 \
            ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5
set aItems $sql_query_select ($(aQuery))
set position 0
for id in $(aItems)

   #---To see an announcement, it must pass the following conditions:
   #   1. Not already seen this session.
   if $tablefind ($(id) $(ann_seen))
      continue
   end

   #---2. Has a non-empty conf var
   set ann $site_data ($(ann_cnum) - ann$(id))
   if $empty ($(ann))
      continue
   end


   #---3. start-date <= now <= stop-date
   if $not ($between ($word(2 $(ann))  $time()   $word(3 $(ann))))

      #---If expired, clear out conf var (faster for everyone else!)
      if $greater ($time() $word (3 $(ann)))
         eval $set_site_data ($(ann_cnum) - ann$(id))
      end
      continue
   end

   #---4. Not in user's "seen" (don't show me again) list.
   if $tablefind ($(id) $user_data ($userid() ann_seen))
      continue
   end

   #---Now we must parse the 'audience' string, and see if the
   #   announcer is allowed each/any part of the string.
   set ann_id $word (1 $(ann))
   set show_ann 0
   set mgr %mgr_in_group ($userid() systemBulletins)
   for audience in $rest (5 $(ann))
      #---'*' must be manager & 512.
      if $tablefind ($(audience) * %)
         if $(mgr)
            set show_ann 1
            break
         end
      end

      #---'<group' must be manager & 512, user must be in group.
      elif $greater ($max ($str_index (->    $(audience)) \
                           $str_index (-&gt; $(audience))) 0)
         set group $str_replace (->    * $(audience))
         set group $str_replace (-&gt; * $(group))
         set group $replace (* 32 $(group))
         if $(mgr)
            if $gt_equal ($group_access ($userid() $(group)) $priv(minimum))
               set show_ann 1
               break
            end
         end
      end

      #---Anything else must be a conference name (we hope!), and
      #   ann_id must be an organizer or a manager.
      else
         set ann_mem $word (1 $cl_list ($(audience)) 0)
         if $(ann_mem)
            if $or ($(mgr) \
               $gt_equal (%access($(ann_mem) $(ann_id)) $priv(organizer)) )
               if %access($(ann_mem))
                  set show_ann 1
                  break
               end
            end
         end
      end
   end

   #---If any one of the audience matches worked, show the announcement!
   if $(show_ann)
      set position $plus($(position) 1)
      include $(main)/announce.cml $(id) $(position)
      set ann_seen $(ann_seen) $(id)
   end
end

#-------------------------------------------------------------
