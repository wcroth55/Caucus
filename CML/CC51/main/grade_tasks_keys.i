#
#--- grade_tasks_keys.i   Get the task keys for a conference.
#
#    Gets the list of task keys for a given conference, in a form
#    suitable for use in an "IN (a, b, ...)" query clause.
#
#    Input:  $inc(1)   conference number
#
#    Output: variable 'keys'
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#------------------------------------------------------------

set keys
set h $sql_query_open (SELECT taskkey FROM grade_tasks WHERE cnum=$inc(1))
while $sql_query_row ($(h))
   set keys $(keys), $(taskkey)
end
eval $sql_query_close ($(h))

set keys $str_sub (1 1000000 $(keys))
#------------------------------------------------------------
