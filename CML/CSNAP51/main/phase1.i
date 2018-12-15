#
#---PHASE1.I   Phase 1 of CSNAP.  Ask user for list of conferences.
#
#   Purpose:
#
#   How it works
#      CSNAP prompts the user for a list of conferences to include
#      in the snapshot.  
#
#   Input: from user
#
#   Output: sets 'cnums' to list of conference numbers of conferences
#      that will be included in the snapshot.
#
#   Notes:
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR 11/01/05 Improve handling of unique conf name which is a substr of others
#==========================================================================

#-------Part I: select conferences.
"
"Please enter the name(s) of the conference(s) that you
"wish to include in the snapshot, in the order in which you
"want them to appear on the Caucus Center page.  
"
"Press RETURN an extra time when you are done.

while 1
   set cnums

   while 1
      set conf $readterm()
      if $empty ($(conf))
         break
      end
      set conf $replace (32 _ $(conf))
   
      set cnum $cl_list ($(conf))
      if $empty ($(cnum))
         "   There is no conference called '$(conf)'.  Please try again.
      end
      elif $not_empty ($word (2 $(cnum)))
         set ambiguous 1
         for cn in $(cnum)
            "Comparing '$(conf)' to '$cl_name($(cn))'...
            if $equal ($(conf) $cl_name($(cn)))
               set cnums $(cnums) $(cn)
               set ambiguous 0
               break
            end
         end
         if $(ambiguous)
         "   Several conferences begin with '$(conf)'.  Please be more specific.
            "Skipping '$(conf)', match is ambiguous.
         end
         else
            "   Found '%displayed_conf_name ($(cn))'.
         end
      end
      elif $tablefind ($(cnum) $(cnums))
         "   Already included '%displayed_conf_name ($(cnum))'.
      end
      else
         "   Found '%displayed_conf_name ($(cnum))'.
         set cnums $(cnums) $(cnum)
      end
   end
   
   if $empty ($(cnums))
      "You have not selected any conferences.  Goodbye.
      quit
   end

   "You have selected the following conferences:
   for cnum in $(cnums)
      "   %displayed_conf_name ($(cnum))
   end
   "
   while 1
      "Is this correct? (y/n)
      set answer $str_sub (0 1 $lower($readterm()))
      if $or ($equal ($(answer) y)  $equal ($(answer) n))
         break
      end
   end

   if $equal ($(answer) y)
      break
   end
   "
   "Please re-enter your entire list of conferences.
end
"
