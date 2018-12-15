#
#---userids_delete.i   One-stop shop to completely delete a list of userids.
#
#   Input:
#      $inc(1)   name of variable containing list of userids.
#
#   Output: 
#      deleted   number of userids successfully deleted
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/07/2005 New script.
#: CR 11/07/2005 Delete from grouprules, too.
#: CR 01/08/2006 Delete USER001 directory.
#: CR 02/27/2006 Cannot delete one's own self!
#-----------------------------------------------------------------------

set udels
set comma
set dtime   $time()
set ptime   $minus ($pagetime() 20)
set deleted 0

if $empty($(iface))
   set iface CC51
end

for uid in $($inc(1))
   if $equal ($(uid) $userid())
      continue
   end
   set deleted $plus($(deleted) 1)

   #---Add this id to SQL mass deletion list.
   set udels $(udels)$(comma) '$(uid)'
   set comma ,

   #---"Resign" user from all conferences they belong to
   set q DELETE FROM resps_seen WHERE userid='$(uid)'
   set h $sql_query_open ($(q))
   eval  $sql_query_close($(h))

   #---Log deluser event.
   eval $logevent (deluser $(iface) 0 0 0 0 0 0 $(uid))

   #---Delete user from babylon.  (Don't worry about this for now.)
   #if $equal ($(error) 0)
   #   eval $xshell (0 0 1 \
   #           /bin/nice $caucus_path()/SCRIPTS/babylon.sh $config(java) \
   #           $caucus_path()/public_html/BABYLON  $inc(1))
   #end

   #---Delete user from "external" integrated application.
   include $(main)/intg_delete.i $(uid)

   #---Delete user from local authentication source.
   set userid_auth $user_data ($(uid) auth_source)
   if $pw_can_delete($(userid_auth))
      set error $pw_delete ($(uid) $(userid_auth))
   end

   #---Delete old-style USER001 directory.
   eval $deldir ($caucus_path()/USER001/$(uid))

   #---If we've already taken more than ptime seconds, then
   #   stop processing userids, and finish up with the SQL mass deletions.
   if $greater ($minus($time() $(dtime)) $(ptime))
      break
   end
end

#---Mass deletion of all the userids from the SQL tables.
eval %sql(DELETE FROM confhidden   WHERE userid in ($(udels)))
eval %sql(DELETE FROM grade_scores WHERE userid in ($(udels)))
eval %sql(DELETE FROM user_data    WHERE userid in ($(udels)))
eval %sql(DELETE FROM groups       WHERE userid in ($(udels)))
eval %sql(DELETE FROM grouprules   WHERE userid in ($(udels)))
eval %sql(DELETE FROM user_info    WHERE userid in ($(udels)))
eval %sql(DELETE FROM user_in_conf WHERE userid in ($(udels)))

#-----------------------------------------------------------------------
