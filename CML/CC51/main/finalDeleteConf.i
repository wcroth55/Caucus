#
#---finalDeleteConf.i   Completely and utterly delete an entire conference.
#
#   Input:
#      inc(1)    conference number to delete
#
#   Output:
#      result    empty if successful
#                error message otherwise
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 09/09/05 New file.
#: CR 10/28/05 Delete from conforder, confhidden tables.
#: CR 04/24/10 rewrite from old conf_delete.
#-------------------------------------------------------------

set result

set fname filesafe_$lower($cl_name($inc(1)))

if $co_remove($inc(1))

   #---Delete any gradebook scores and tasks for this conference.
   include $(main)/grade_tasks_keys.i $inc(1)

   set h $sql_query_open  (DELETE FROM grade_scores WHERE taskkey IN ($(keys)))
   eval  $sql_query_close ($(h))
   set h $sql_query_open  (DELETE FROM grade_tasks  WHERE taskkey IN ($(keys)))
   eval  $sql_query_close ($(h))

   set h $sql_query_open  (DELETE FROM assignments  where cnum=$inc(1))
   eval  $sql_query_close ($(h))
   
   #---Delete the conference variables, task links, name, and group(s).
   eval %sql(DELETE FROM site_data  WHERE cnum='$inc(1)')
   eval %sql(DELETE FROM task_links WHERE cnum='$inc(1)')
   eval %sql(DELETE FROM confs      WHERE cnum='$inc(1)')
   eval %sql(DELETE FROM grouprules WHERE owner='CONF' AND name='$inc(1)')
   eval %sql(DELETE FROM groups     WHERE owner='CONF' AND name='$inc(1)')
   eval %sql(DELETE FROM confhidden WHERE cnum='$inc(1)')
   eval %sql(DELETE FROM conforder  WHERE cnum='$inc(1)')

   #---Delete the filesafe
   include $(main)/filesafe_del.i $(fname)
end

else
   set result <FONT COLOR=RED>Error deleting conference number $inc(1).</font>
end

#------------------------------------------------------
