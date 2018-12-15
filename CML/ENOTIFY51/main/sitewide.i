#
#---sitewide.i   Handle site-wide email notification, conf by conf.
#
#   Global vars:
#      cnum        current conference number
#      site_users  list of uids that have already received sitewide
#                  notifications.
#
#:CR 01/07/04 New file.
#:CR  8/10/05 Skip if user chose "suspend notifications".
#:CR 12/23/05 Fix expansion of %name macro.
#:CR 06/28/08 Skip interface_storage confs!
#--------------------------------------------------------------------------

#---Ignore "interface_storage" confs!  (should be for non-managers...)
if $equal ($str_index (interface_storage_ $cl_name($(cnum))) 0)
   return
end

#---Build the list of members of this conference (who have not
#   already been notified).
set members
for uid in $peo_members ($(cnum))
   if $not ($tablematch ($(uid) $(site_users)))
      set members $(members)  $(uid)
   end
end
#eval $append ($(sitelog) cnum=$(cnum), members=$(members)$newline())

#---For each member...
for uid in $(members)
   if 0$user_info ($(uid) ur_suspend)
#     eval $append ($(sitelog) ---uid=$(uid) suspended$newline())
      continue
   end

   #---See if sitewide notification is on and this is an active day/hour.
   set iface $word (1 $user_data ($(uid) ur_faviface) $(dir))

   if $not (0$site_data (0 - en_on_$(iface)))
#     eval $append ($(sitelog) ---uid=$(uid) no en_on$newline())
#     " no en_on
      continue
   end

   #---Is this a notification hour for this interface?
   set hours $site_data (0 - en_hours-$(iface))
   if $not ($tablefind ($(hour) $(hours)))
#     eval $append ($(sitelog) ---uid=$(uid) no hours: hour='$(hour)', hours=en_hours-$(iface), table='$(hours)'$newline())
      continue
   end

   #---Is this a notification day for this interface?
   set days $site_data (0 - en_dates-$(iface))
   if $word (1 $(days))
      if $not ($tablefind ($(wday) $rest(2 $(days)) ))
#        eval $append ($(sitelog) ---uid=$(uid) no wdays$newline())
         continue
      end
   end
   else
      if $not ($tablefind ($(day)  $rest(2 $(days)) ))
#        eval $append ($(sitelog) ---uid=$(uid) no mdays$newline())
         continue
      end
   end

   #---Is anything in this conf new to 'uid' ?
   set anynew 0
   include $(main)/itemListOf.i $(cnum) $(uid) new 0 0 0
   set anynew $greater ($sizeof($(itemList)) 0)

   #---If there's any new anything, send an e-mail.
   if $(anynew)
#     eval $append ($(sitelog) ---uid=$(uid) sent to $user_info($(uid) e-mail)$newline())
      eval $output ($(tfile) 644)
      eval $mac_define (name \$user_info($(uid) fname) \
                             \$user_info($(uid) lname))
      "Subject: $site_data (0 - en_subject_$(iface))
      "To: $user_info ($(uid) email)
      "
      "$mac_expand($site_data (0 - en_notetext_$(iface)))

      eval $output()
      eval $xshell (0 1 1 $(sendmail) $user_info ($(uid) email) <$(tfile) )
      eval $delfile  ($(tfile))
      set sent $plus ($(sent) 1)
      set site_users $(site_users) $(uid)
   end
   else
#     eval $append ($(sitelog) ---uid=$(uid) nothing new$newline())
   end

end

#----------------------------------------------------------------------------
