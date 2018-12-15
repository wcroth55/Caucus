#
#---namecascade_build.i  Rebuild the name_cascade table as needed
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 09/18/05 New file.
#: CR 11/23/05 Complete rewrite using $build_name_cascade() function.
#-------------------------------------------------------------------

#---Check if we should skip rebuilding the name cascade altogether.
set q SELECT value FROM site_data WHERE name='skip_cascade_build'
if $word (1 $sql_query_select ($(q)) 0)
   return
end

#---The name_cascade table keeps track of nice blocks of names
#   (user first names, last names, userids, conf names) for 
#   the various pull-down menus.  It can get out of date for
#   several reasons:
#      1. The mix of names changes enough to make the blocks lop-sided
#      2. New names are added that are outside the top/bottom borders
#         (rare but a problem when it does happen!)

set rebuild 0

#---Case 1: update name_cascade once a day.
set age $word (1 0$site_data (0 - name_cascade_age))
if $greater ($minus ($time() $(age)) 86400)
   set rebuild 1
end

#---Checking for case 2.  Find top/bot of fname, lname, userid.  Is
#   anything outside of them?
if $not($(rebuild))
   for field in fname lname userid
      set q SELECT name FROM name_cascade WHERE type='$(field)' ORDER BY name 
      set top $sql_query_select($(q)      LIMIT 1)
      set bot $sql_query_select($(q) DESC LIMIT 1)
      
      set q SELECT count(*) FROM user_info WHERE  $(field) != ''  \
               AND ($(field) > '$(bot)' OR $(field) < '$(top)')
      set outside $sql_query_select($(q))
      if $greater ($(outside) 0)
         set rebuild 1
         break
      end
   end
end

#---Continuing case 2.  Find top/bot of conf names, anything outside them?
if $not($(rebuild))
   set q SELECT name FROM confs WHERE cnum > 0 ORDER BY NAME
   set top $sql_query_select($(q)      LIMIT 1)
   set bot $sql_query_select($(q) DESC LIMIT 1)
   set q SELECT count(*) FROM confs \
          WHERE (name > '$(bot)' OR name < '$(top)')
   set outside $sql_query_select($(q))
   set rebuild $greater ($(outside) 0)
end

#---If we need to rebuild name_cascade, make sure only one process
#   messes with it at a time.
if $(rebuild)
   count tries 0 6
      if $lock (11 20)
         eval $build_name_cascade(50)
         eval $set_site_data (0 - name_cascade_age $time() $dateof($time()))
         eval $unlock (11)
         break
      end
   end
end
