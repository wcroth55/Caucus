#
#---man_andusers.i    Calculate "AND userid in..." part of SQL in man_statwizf
#
#   inputs:
#      $form(who)    
#      $form(users)
#
#   output:
#      variable 'list',      blank separated list of users.
#      variable 'and_users', suitable for use in SQL query.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 07/26/03 New script.
#: CR 04/25/05 Add 'list' output variable.
#--------------------------------------------------------------------

set and_users

#---If specific user list is "(all those users)"...
if $equal (x$word(1 $form(users)) x-)

   #---And the group/conf is "entire site", then don't AND against users at all.
   if $equal ($form (who) -2)
      set list $all_users()
      return
   end

   set query \
          SELECT DISTINCT g.userid FROM groups g, user_info u \
           WHERE g.userid = u.userid AND u.active >= 10 \
             AND g.owner='$(egType)' AND g.name='$(egName)' \
             AND g.access >= $priv(minimum)  AND g.active > 0 \
           ORDER BY u.lname, u.fname
   set list $sql_query_select ($(query))
end

#---If it's an actual specific list of users, use that!
else
   set list $form (users)
end

#---Built the list of quoted userids.
for user in $(list)
   set and_users $(and_users) '$(user)'
end

#---And turn it into an AND clause.
set and_users AND e.userid in ($replace (32 , $(and_users)))

#--------------------------------------------------------------------
