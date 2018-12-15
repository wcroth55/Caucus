#
#--- ONNOW_CALC.I - Calculate list of userids currently "on now".
#
# Arguments:
#   inc(1)    force recalculation
#             1 => force recalculation immediately
#             0 => recalculate only every 5 minutes
# 
# Output:
#   onnow_users         unsorted list of userids "on now":
#                         as pairs of (userid, cnum)
#
# State variables:
#   onnow_lastchecked   epoch time "on now" status last checked
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 12/26/03 New file, derived from old onnow.i
#: CR 10/25/04 Add cnum element to onnow_users output.
#------------------------------------------------------------------------

#---If forced recalculation, or last-checked time is > 5 minutes old...
if $or ($greater (0$inc(1) 0) \
        $greater ($time() $plus (0$(onnow_lastchecked) 300)) )

   #---onnow_timeout controls when a person is no longer considered "on now".
   set onnow_timeout $word (1 $config(onnow_timeout) $timeout())
   
   #---Loop through the files in $caucus_path()/MISC/ONNOW, which are created
   #   when a person logs in and updated when they do something.  Clear out any
   #   that have been inactive longer than the timeout period.
   set onnow_users
   for onnow_whois in $dirlist(0 $caucus_path()/MISC/ONNOW)
   
      #---open the current marker file
      set onnow_file $open($caucus_path()/MISC/ONNOW/$(onnow_whois) r) 
   
      #---if the open was successful read marker file's contents into a
      #   variable, then close it
      if $(onnow_file)
         eval $readln($(onnow_file) onnow_entry)
         eval $readln($(onnow_file) onnow_cnum)
         eval $close ($(onnow_file))
         set onnow_cnum $word (1 $(onnow_cnum) x)
   
         #---check marker and remove it if older than $(onnow_timeout) seconds
         if $less ($word (1 0$(onnow_entry)) $minus ($time() $(onnow_timeout)))
            eval $delfile($caucus_path()/MISC/ONNOW/$(onnow_whois))
         end
   
         #---else this user is still online, add them to the list
         else
            set onnow_users $(onnow_users) $(onnow_whois) $(onnow_cnum)
         end
      end
   end

   set onnow_lastchecked $time()
end
#------------------------------------------------------------------------
