#
#---MAN_GRPLIST.I    Calculate list of people (for man_grpedit.cml)
#
#   inc(1)    kind of people selection:
#                 0    no people
#                -1    people that match $inc(3) in any way
#                -2    everyone on the site
#                -3    in groups in $inc(2)
#                -4    in all groups
#                n>0   members of conference n
#                name  members of group 'name'
#            
#   inc(2)    group list (group names separated by commas, no spaces)
#
#   inc(3)    data for people selection
#
#   output: variable 'list' of userids (sorted by userid), and
#             'uniq'ed (duplicates removed)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 04/04/03 New script.
#: CR 12/22/03 Sort by last name.
#----------------------------------------------------------------------

set list0
set list

if $numeric ($inc(1))
   if   $equal ($inc(1) 0)
      return
   end
   
   elif $equal ($inc(1) -1)
      set list0 $userids_byname (0 $inc(3))
   end
   
   elif $equal ($inc(1) -2)
      set list0 $users_byid()
      include $(main)/groups.i
      for group in $(groups)
         include $(main)/man_grpload.i group
      end
   end

   elif $equal ($inc(1) -3)
      for group in $replace (, 32 $inc(2))
         include $(main)/man_grpload.i group
      end
   end

   elif $equal ($inc(1) -4)
      include $(main)/groups.i
      for group in $(groups)
         include $(main)/man_grpload.i group
      end
   end

   else
      #---Results already sorted by last name!
      set list $peo_members ($inc(1))
      return
   end
end

else
   #---group name
   set group $inc(1)
   include $(main)/man_grpload.i group

   set fd $open ($caucus_path()/GROUPS/$inc(1) r)
   while $readln ($(fd) line)
      if $not_equal ($str_sub (0 1 $(line)) #)
          set list0 $(list0) $word (1 $(line))
      end
   end
   eval $close ($(fd))
end

#---Sort and 'uniq' the resulting userids, by lastnames(!), and put in 'list'.
set lastnames
for uid in $(list0)
   set fullname  %per_name($(uid))
   set lastnames $(lastnames) $user_info ($(uid) lname)
end
set sort_indices $gen_sort (1 $(lastnames))
set list
set prev ___
for gdex in $(sort_indices)
   set this $word ($(gdex) $(list0))
   if  $not_equal ($(prev) $(this))
      set list $(list) $word ($(gdex) $(list0))
      set prev $(this)
   end
end
#----------------------------------------------------------------------
