#
#---HEADER.I
#
#: CR 03/30/00 13:03 Add textbox field type "_x".
#: CR 03/25/01 16:00 Add [urc] argument to make report registration-specific
#: CR 03/05/02 10:20 Add RegTime.
#-------------------------------------------------------------------

set line

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
      set line $(line)userid$char(9)
   end

   #---First Name
   elif $equal ($(type) fn)
      set line $(line)FirstName$char(9)
   end

   #---Last Name
   elif $equal ($(type) ln)
      set line $(line)LastName$char(9)
   end

   #---Brief intro
   elif $equal ($(type) bi)
      set line $(line)Intro$char(9)
   end

   #---Pulldown field, Text field, State List
   elif $tablefind ($(type) _p _t _c _x sl)
      set line $(line)$rest(9 $(urc))$char(9)
   end

   #---E-mail
   elif $equal ($(type) em)
      set line $(line)E-mail$char(9)
   end

   #---Home page
   elif $equal ($(type) hp)
      set line $(line)HomePage$char(9)
   end

   #---Picture URL
   elif $equal ($(type) pu)
      set line $(line)Picture$char(9)
   end

   #---Phone
   elif $equal ($(type) tp)
      set line $(line)Phone$char(9)
   end
end

set line $(line)RegDir$char(9)
set line $(line)RegTime$char(9)

"$(line)

#-------------------------------------------------------------------
