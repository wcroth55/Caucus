#
#---man_statwizextrasf.i
#
#   Add-on module "extras" for statistics wizard, form processing section.
#
#--------------------------------------------------------------------------

#---by Conference report counts up 'accesses' (leftconf events)
#   for selected users across selected conferences.
if $equal ($form(report) c)
   set sql SELECT e.userid, e.conf, count(*) \
             FROM events e, user_info u \
            WHERE e.userid = u.userid  \
              AND e.event = 'leftconf' \
              AND e.time >= '$form(d0year)-$(d0mNum)-$form(d0day)' \
              AND e.time <  '$form(d1year)-$(d1mNum)-$form(d1day)' \
              $(and_confs) \
              $(and_users) \
            GROUP BY e.userid, e.conf \
            ORDER BY u.lname, u.fname

   #---Output display header row.
   include $(main)/statwizopen.i
#  include $(main)/statwizhead.i 10 $quote($(sql))
#  include $(main)/statwizhead.i
   include $(main)/statwizhead.i 5  \
      Accesses&nbsp;per&nbsp;User&nbsp;per&nbsp;Conference&nbsp;\
              $form(d0year)-$(d0mNum)-$form(d0day)&nbsp;thru&nbsp;\
              $form(d1year)-$(d1mNum)-$form(d1day)
   include $(main)/statwizhead.i
   include $(main)/statwizhead.i 2  User
   for conf in $form(confs)
      include $(main)/statwizhead.i 1  $quote(%displayed_conf_name ($(conf)))
   end
   include $(main)/statwizhead.i 1  TOTAL
   include $(main)/statwizhead.i

   #---Take the doubly-grouped-by list of results, and turn them into
   #   rows of 'by userid'.  We'll simulate storing the counts in an array
   #   by using variable name dereferencing (ugh).
   set lastid _
   set parity      1
   set peopleCount 0
   for row in $sql_select ($(sql))
      set row_result $sql_row ($(row))
      set thisid $sql_col($word (1 $(row_result)))

      #---If userid changed, we finished a row...
      if $not_equal ($(thisid) $(lastid))

         #---Output the old row (if any).
         if $not_equal ($(lastid) _)
            include $(main)/statwizcout.i
         end

         #---Reset 'last' userid to this userid, and clear out simulated array.
         set lastid $(thisid)
         set total 0
         for conf in $form(confs)
            set count$(conf)
         end
      end

      #---Save count for this id / this conference in simulated array.
      set countname count$sql_col($word(2 $(row_result)))
      set count          $sql_col($word(3 $(row_result)))
      set total  $plus ( $(count) $(total))
      set $(countname)   $(count)
   end

   #---Output very last row.
   set lastid $(thisid)
   include $(main)/statwizcout.i
   include $(main)/statwizclose.i
end

#---by Post report counts up logins and actual item/response adds
#   across set of confs, per user.  Also calculates % total.
if $equal ($form(report) p)
   set sql SELECT count(*) \
             FROM events e \
            WHERE e.event in ('additem', 'addresp') \
              AND e.time >= '$form(d0year)-$(d0mNum)-$form(d0day)' \
              AND e.time <  '$form(d1year)-$(d1mNum)-$form(d1day)' \
              $(and_confs) \
              $(and_users)
   for row in $sql_select ($(sql))
      set total $sql_col ($word (1 $sql_row ($(row))))
   end

#  set sql SELECT userid, event, count(*) \
#            FROM events \
#           WHERE event in ('additem', 'addresp', 'login') \
#             AND time >= '$form(d0year)-$(d0mNum)-$form(d0day)' \
#             AND time <  '$form(d1year)-$(d1mNum)-$form(d1day)' \
#             $(and_confs) \
#             $(and_users) \
#           GROUP BY userid, event \
#           ORDER BY userid

   set sql SELECT e.userid, e.event, count(*) \
             FROM events e, user_info u \
            WHERE e.userid = u.userid \
              AND e.event in ('additem', 'addresp', 'login') \
              AND e.time >= '$form(d0year)-$(d0mNum)-$form(d0day)' \
              AND e.time <  '$form(d1year)-$(d1mNum)-$form(d1day)' \
              $(and_confs) \
              $(and_users) \
            GROUP BY e.userid, e.event \
            ORDER BY u.lname, u.fname

   #---Output display header row.
   include $(main)/statwizopen.i
   include $(main)/statwizhead.i 4  Site&nbsp;logins&nbsp;&amp;&nbsp;Posts&nbsp;by&nbsp;User,&nbsp;\
           $form(d0year)-$(d0mNum)-$form(d0day)&nbsp;thru&nbsp;\
           $form(d1year)-$(d1mNum)-$form(d1day)
   include $(main)/statwizhead.i
   include $(main)/statwizhead.i 2  User
   include $(main)/statwizhead.i 1  Logins align=right
   include $(main)/statwizhead.i 1  Posts align=right
   include $(main)/statwizhead.i 1  $quote(&nbsp;\% all Posts) align=right
   include $(main)/statwizhead.i

   #---Take the doubly-grouped-by list of results, and turn them into
   #   rows of 'by userid'.
   set lastid _
   set peopleCount 0
   set parity      1
   for row in $sql_select ($(sql))
      set row_result $sql_row ($(row))
      set thisid $sql_col($word (1 $(row_result)))

      #---If userid changed, we finished a row...
      if $not_equal ($(thisid) $(lastid))

         #---Output the old row (if any).
         if $not_equal ($(lastid) _)
            include $(main)/statwizpout.i
         end

         #---Reset 'last' userid to this userid, and clear out counts.
         set lastid $(thisid)
         set count_login   0
         set count_additem 0
         set count_addresp 0
      end

      #---Save count of this event type.
      set event           $sql_col($word(2 $(row_result)))
      set countname count_$(event)
      set $(countname)    $sql_col($word(3 $(row_result)))
   end

   #---Output very last row.
   set lastid $(thisid)
   include $(main)/statwizpout.i
   include $(main)/statwizclose.i
end

#---by day of the Week...
if $equal ($form(report) w)
   set sql SELECT e.userid, DAYOFWEEK(e.time), count(*) \
             FROM events e, user_info u \
            WHERE e.userid = u.userid  \
              AND e.event = 'leftconf' \
              AND e.time >= '$form(d0year)-$(d0mNum)-$form(d0day)' \
              AND e.time <  '$form(d1year)-$(d1mNum)-$form(d1day)' \
              $(and_confs) \
              $(and_users) \
              GROUP BY e.userid, DAYOFWEEK(e.time) \
              ORDER BY u.lname, u.fname

   #---Output display header row.
   include $(main)/statwizopen.i
   include $(main)/statwizhead.i 8  Accesses&nbsp;by&nbsp;Week&nbsp;Day,&nbsp;\
              $form(d0year)-$(d0mNum)-$form(d0day)&nbsp;thru&nbsp;
              $form(d1year)-$(d1mNum)-$form(d1day)
   include $(main)/statwizhead.i
   include $(main)/statwizhead.i 2  User
   for day in Mon Tues Wed Thu Fri Sat Sun
      include $(main)/statwizhead.i 1  $(day)
   end
   include $(main)/statwizhead.i

   #---Take the doubly-grouped-by list of results, and turn them into
   #   rows of 'by userid'.
   set lastid _
   count day 1 7
      set count$(day)
   end
   set parity      1
   set peopleCount 0
   for row in $sql_select ($(sql))
      set row_result $sql_row ($(row))
      set thisid $sql_col($word (1 $(row_result)))

      #---If userid changed, we finished a row...
      if $not_equal ($(thisid) $(lastid))

         #---Output the old row (if any).
         if $not_equal ($(lastid) _)
            include $(main)/statwizwout.i
         end

         #---Reset 'last' userid to this userid, and clear out counts.
         set lastid $(thisid)
         count day 1 7
            set count$(day)
         end
      end

      #---Save count of this day.
      set day           $sql_col($word(2 $(row_result)))
      set countname     count$(day)
      set $(countname)  $sql_col($word(3 $(row_result)))
   end

   #---Output very last row.
   set lastid $(thisid)
   include $(main)/statwizwout.i
   include $(main)/statwizclose.i
end

#---by Day...
if $equal ($form(report) d)
   set sql SELECT e.userid, \
                  TO_DAYS(e.time) - \
                  TO_DAYS('$form(d0year)-$(d0mNum)-$form(d0day)') del, \
                  count(*) FROM events e, user_info u \
            WHERE e.userid = u.userid \
              AND e.event = 'leftconf' \
              AND e.time >= '$form(d0year)-$(d0mNum)-$form(d0day)' \
              AND e.time <  '$form(d1year)-$(d1mNum)-$form(d1day)' \
              $(and_confs) \
              $(and_users) \
              GROUP BY e.userid, \
                       TO_DAYS(e.time) - \
                       TO_DAYS('$form(d0year)-$(d0mNum)-$form(d0day)') \
              ORDER BY u.lname, u.fname

   #---Output display header row.
   include $(main)/statwizopen.i
   include $(main)/statwizhead.i 8  Accesses&nbsp;per&nbsp;Day
   include $(main)/statwizhead.i 
   include $(main)/statwizhead.i 2  User

   #---Output header row of 'month/day' numbers for each column.
   set t0 %epochOfyyyy ($form(d0year)-$(d0mNum)-$form(d0day))
   set t1 %epochOfyyyy ($form(d1year)-$(d1mNum)-$form(d1day))
   set counter 0
   while $less ($(t0) $plus ($(t1) 86000))
      set dateform $str_sub (5 1000 %yyyymmddOf ($dateof($(t0))))
      set dateform $replace (- / $(dateform))
      if $equal ($str_sub (0 1 $(dateform)) 0)
         set dateform $str_sub (1 1000 $(dateform))
      end
      include $(main)/statwizhead.i 1  $(dateform)
      set t0 $plus ($(t0) 86400)
      set count$(counter)
      set counter $plus ($(counter) 1)
   end
   include $(main)/statwizhead.i

   #---Take the doubly-grouped-by list of results, and turn them into
   #   rows of 'by userid'.
   set lastid _
   set parity      1
   set peopleCount 0
   for row in $sql_select ($(sql))
      set row_result $sql_row ($(row))
      set thisid $sql_col($word (1 $(row_result)))

      #---If userid changed, we finished a row...
      if $not_equal ($(thisid) $(lastid))

         #---Output the old row (if any).
         if $not_equal ($(lastid) _)
            include $(main)/statwizdout.i
         end

         #---Reset 'last' userid to this userid, and clear out counts.
         set lastid $(thisid)
         count day 0 $(counter)
            set count$(day)
         end
      end

      #---Save count of this day.
      set day           $sql_col($word(2 $(row_result)))
      set countname     count$(day)
      set $(countname)  $sql_col($word(3 $(row_result)))
   end

   #---Output very last row.
   set lastid $(thisid)
   include $(main)/statwizdout.i

   include $(main)/statwizclose.i
end
