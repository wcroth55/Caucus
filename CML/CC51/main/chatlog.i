#
#---chatlog.i    includecml definition of %chatlog macro.
#
#   inc(1)  usual $arg(3)...$arg(8) string.
#   inc(2)  room
#   inc(3)  date
#   inc(4)  time
#   inc(5-9)  link text
#
#: CR 08/18/03 New file.
#------------------------------------------------------------------------

#---Determine room name
if $equal ($inc(2) 0)
   set room $cl_name ($word (1 $replace (+ 32 $inc(1))))
end
elif $numeric ($inc(2))
   set room $cl_name ($inc(2))
end
else
   set room $inc(2)
end

set linktext $inc(5) $inc(6) $inc(7) $inc(8) $inc(9)

#---Assemble the list of epoch times of the chatlogs for the chosen room.
set chatlist
for chatlog size in $dirlist (1 $caucus_path()/CHATLOGS)
   if $and ($equal ($str_index ($(room). $(chatlog)) 0) \
            $greater ($(size) 0))
      set chatlist $(chatlist) $word (2 $replace (. 32 $(chatlog)))
   end
end

if $not_equal ($inc(3) 0)
   set epochhi  $plus  ($epoch ($inc(3) $inc(4)) 60)
end

for epoch in $num_sort (-1 $(chatlist))
   if $or ($equal ($inc(3)$inc(4) 00)    $greater (0$(epochhi) $(epoch)) )
      "<a href="$(mainurl)/chatlog.cml?$(nch)+$(nxt)+$inc(1)+\
                $(room).$(epoch).chatlog">$(linktext)</a>
      quit
   end
end

"(no matching chat log)
#------------------------------------------------------------------------
