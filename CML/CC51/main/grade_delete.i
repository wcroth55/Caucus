#
#--- grade_delete.i   Delete a userid from the grade_scores table.
#
#    inc(1)   userid to delete
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#---------------------------------------------------------------

if $not_empty ($inc(1))
   set h $sql_query_open (DELETE FROM grade_scores WHERE userid='$inc(1)')
   eval  $sql_query_close($(h))
end
