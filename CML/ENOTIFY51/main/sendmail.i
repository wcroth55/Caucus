#
#---SENDMAIL.I   Send e-mail notifications for a list of item groups.
#
#   Invoked as:
#      sendmail.i
#
#   Input variables:
#      inc(1)    name of variable containing duplet list of item groups
#      inc(2)    overriden list -- name of variable containing list 
#                  of item groups to ignore because they were overridden
#      uid       userid
#      tfile     full path to temp file for building messages
#      cnum      conference name
#      sweb      sweb parameter for caucus urls
#      swebsock  swebsock program
#      dir       normal caucus interface directory (e.g. SP41)
#      sendmail  full path to sendmail program
#      sent      counter of number of messages sent
#
#   Purpose:
#
#   How it works:
#
#   Known bugs:
#
#   History of revisions:
#: CR  3/12/99 First version.
#: CR 10/30/02 Update for 4.4, includes "@interface" in URLs.
#: CR 12/29/03 Delete temp file $(tfile) after sending;
#              add handling of login URL; trusted user handling.
#: CR 10/07/05 C5 update, check $it_visib(), use $user_data/info throughout.
#: CR 03/13/07 Major rewrite for 5.1.
#----------------------------------------------------------------------------

#---trusted_user is 1 if we can use a "sendmail trusted user" to
#   forge the "From:" address for this mail to be the conference organizer.
#   (On most systems, see /etc/mail/trusted_users.)
#   If for some reason this does not work, set it to "0" for the best
#   alternative.
set trusted_user 0

set org_mail $user_info ($co_org($(cnum)) email)
if $empty ($(org_mail))
   set org_mail $userid()
end

#---For each item group that is active *now*...
for tc triplet in $($inc(1))
#  "tc=$(tc), triplet=$(triplet), [triplet]='$($(triplet))'

   #---Skip overridden groups
   if $tablefind ($(tc) $($inc(2)))
      continue
   end

   #---Accumulate list of items with new resps, and new items.
   set items 0
   for ic ii ir in $($(triplet))
      set items $(items), $(ii)
   end

   set createEmailHeader 1
   set cname %displayed_conf_name ($(cnum))

   #---Get the list of items with new responses, and (if any), email
   #   the user.
   set qRnew  \
       SELECT i.id, s.seen, i.title, i.lastresp, \
              h.item0, h.item1, h.item2, h.item3, h.item4, h.item5 \
         FROM items i, conf_has_items h, resps_seen s \
        WHERE h.cnum=$(cnum)      AND h.items_id = i.id  \
          AND h.hidden=0  AND h.retired=0  AND h.deleted=0 \
            AND s.items_id = i.id   AND s.userid='$(uid)' \
            AND i.id IN ($(items)) \
            AND (s.forgot IS NULL  OR s.forgot=0) \
            AND i.lastresp > s.seen  \
         ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5

   set hRnew $sql_query_open ($(qRnew))
   while     $sql_query_row  ($(hRnew))
      if $(createEmailHeader)
         set createEmailHeader 0
         include $(en)/emailHeader.i
         "
         "There are new responses on items:
      end

      set ilabel $item_tuplet($(item0) $(item1) $(item2) $(item3) \
                              $(item4) $(item5))
      set indent $mult(2 $minus($sizeof($replace(. 32 $(ilabel))) 1))

      if $equal($(indent) 0)
         set line $str_sub (0 60 x$pad($minus(4 $strlen($(ilabel))))$(ilabel) \
                                 $(title) $pad(60))
      end
      else
         set line $str_sub (0 60 x$pad(4)$pad($(indent))$(ilabel) $(title) $pad(60))
      end

      "$str_sub(1 1000 $(line))  ($minus($(lastresp) $(seen)) new)
   end
   eval      $sql_query_close($(hRnew))

   set qInew \
       SELECT i.id, i.title, \
              h.item0, h.item1, h.item2, h.item3, h.item4, h.item5 \
         FROM (conf_has_items h, items i) \
         LEFT JOIN resps_seen s  \
                  ON (s.userid='$(uid)' AND s.items_id = i.id) \
          WHERE h.cnum=$(cnum) AND h.items_id = i.id AND s.items_id IS NULL \
            AND h.hidden=0     AND h.retired=0  AND h.deleted=0 \
            AND i.id IN ($(items)) \
          ORDER BY h.item0, h.item1, h.item2, h.item3, h.item4, h.item5

   set noNewItems 1
   set hInew $sql_query_open ($(qInew))
   while     $sql_query_row  ($(hInew))
      if $(createEmailHeader)
         set createEmailHeader 0
         include $(en)/emailHeader.i
      end
      if $(noNewItems)
         set noNewItems 0
         "
         "There are new items:
      end

      set ilabel $item_tuplet($(item0) $(item1) $(item2) $(item3) \
                              $(item4) $(item5))
      set indent $mult(2 $minus($sizeof($replace(. 32 $(ilabel))) 1))

      if $equal($(indent) 0)
         set line $str_sub (0 60 x$pad($minus(4 $strlen($(ilabel))))$(ilabel) \
                                 $(title) $pad(60))
      end
      else
         set line $str_sub (0 60 x$pad(4)$pad($(indent))$(ilabel) $(title) $pad(60))
      end

      "$str_sub(1 1000 $(line))
   end
   eval      $sql_query_close($(hInew))

   if $not ($(createEmailHeader))
      eval $output()
      eval $xshell (0 1 1 $(sendmail) \
                    $ifthen ($(trusted_user) -f$(org_mail)) \
                    $user_info ($(uid) email) <$(tfile) )
      eval $delfile  ($(tfile))
      set sent $plus ($(sent) 1)
   end

end

#----------------------------------------------------------------------------
