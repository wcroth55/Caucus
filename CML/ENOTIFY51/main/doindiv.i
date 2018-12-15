#
#---DOINDIV.I     Process and send e-notification for a specific
#                 user that has an individual notification schedule.
#
#   Invoked as:
#      doindiv.i
#
#   Input variables:
#      cnum      conference number
#      uid       userid
#      groups    organizer defined item groups (list of duplets of item
#                   group T values and item triplet list variable names)
#
#   Purpose:
#
#   How it works:
#
#   Known bugs:
#
#   History of revisions:
#: CR  3/12/99 11:02 First version.
#: CR  3/27/00 13:32 Use %bad_email_address(). Move mskip build to top of loop.
#: CR  8/01/05 Return if user chose "suspend notifications".
#: CR 12/16/05 ur_suspend is a $user_info() field.
#----------------------------------------------------------------------------

#---Ensure user has e-mail!
set user_addr $user_info ($(uid) email)
if $empty ($(user_addr))
#  "
#  "------------------------
#  "No e-mail for $(uid)
   return
end
if %bad_email_address ($(user_addr))
   return
end
if 0$user_info ($(uid) ur_suspend)
   return
end

#---Ensure user still has access to conference
if $less ($group_access ($(uid) CONF $(cnum)) 10)
#  "
#  "------------------------
#  "No access for $(uid)
   return
end

#---Ensure user is (still) a member of the conference.
#if $empty ($per_lastin ($(uid) $(cnum)))
#   return
#end

#---Build list of individually defined item groups that
#   are active at this time.  (duples of item group T
#   values and item triplet variable names)
set mgroups
set mindex 0
set mskip
for eni in $list_user_data ($(uid) en_items_$(cnum)- )
#  "$(uid) eni=$(eni)
   set dash $str_index (- $(eni))
   set tc   $str_sub   ($plus ($(dash) 1) 1000 $(eni))
   set eni_group $user_data ($(uid) en_items_$(cnum)-$(tc))

   #---If this group overrides an organizer defined group, add the
   #   *overridden* group to mskip.
   if $word (5 $(eni_group))
      set mskip $(mskip) $word (5 $(eni_group))
   end

   #---Is this item group active?
   if $not ($word (1 $user_data ($(uid) en_items_$(cnum)-$(tc))))
      continue
   end

   #---Is this a notification hour for this group?
   set hours $user_data ($(uid) en_hours_$(cnum)-$(tc))
   if $not ($tablefind ($(hour) $rest(2 $(hours))))
      continue
   end

   #---Is this a notification day for this group?
   set days $user_data ($(uid) en_dates_$(cnum)-$(tc))
   if $word (1 $(days))
      if $tablefind ($(wday) $rest(2 $(days)) )
         set mgroups $(mgroups) $(tc) mlist$(mindex)
      end
   end
   else
      if $tablefind ($(day)  $rest(2 $(days)) )
         set mgroups $(mgroups) $(tc) mlist$(mindex)
      end
   end

   #---This group is active now.  Build its triplet list.
   include $(en)/tlist.i mlist$(mindex) eni_group
   set mindex $plus ($(mindex) 1)

end

#---Send mail for individually defined groups
set mempty
set kind indiv
include $(en)/sendmail.i mgroups mempty

#---Send mail for conference-wide groups (minus overridden groups)
set kind minus
include $(en)/sendmail.i groups  mskip

#----------------------------------------------------------------------------
