#
#---DOUSER.I
#
#: CR 03/30/00 13:03 Add textbox field type "_x".
#: CR 03/25/01 16:00 Record registering dir name in ur__reg.
#: CR 03/25/01 16:00 Add [urc] argument to make report registration-specific
#----------------------------------------------------------------

set line

set id $inc(1)
count blk 1 1000

   set urc     $site_var ($(urcbase)$(blk))
   set type    $word (4 $(urc))
   set reqd    $word (6 $(urc))
   set varname $word (7 $(urc))
   if $empty ($(urc))
      break
   end

   if $tablefind ($(type) ht pw pv)
      continue
   end

   #---Userid
   if $equal ($(type) id)
      set line $(line)$inc(1)$char(9)
   end

   #---First Name
   elif $equal ($(type) fn)
      set line $(line)$words(1 $minus($sizeof($per_name($(id))) 1) \
                                              $per_name($(id)))$char(9)
   end

   #---Last Name
   elif $equal ($(type) ln)
      set line $(line)$words($sizeof($per_name($(id))) 1 \
                             $per_name($(id)))$char(9)
   end

   #---Brief intro
   elif $equal ($(type) bi)
      set line $(line)$replace(10 32 $per_intro($(id)))$char(9)
   end

   #---Pulldown field, Text field, State List
   elif $tablefind ($(type) _p _t _c _x sl)
      set line $(line)$replace (10 32 $user_var ($(id) ur_$(varname)))$char(9)
   end

   #---E-mail
   elif $equal ($(type) em)
      set line $(line)$user_var ($(id) e-mail)$char(9)
   end

   #---Home page
   elif $equal ($(type) hp)
      set line $(line)$user_var ($(id) homepage)$char(9)
   end

   #---Picture URL
   elif $equal ($(type) pu)
      set line $(line)$user_var ($(id) picture)$char(9)
   end

   #---Phone
   elif $equal ($(type) tp)
      set line $(line)$per_phone($(id))$char(9)
   end
end

#---Add registration directory, if any.
set line $(line)$user_var ($(id) ur__regdir)$char(9)

#---Add date/time registered, if any.
set line $(line)$user_var ($(id) ur__time)$char(9)

"$(line)

#-------------------------------------------------------------------
