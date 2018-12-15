#
#---conf_delete.i    Delete a conference.
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
#-------------------------------------------------------------

set result

set query UPDATE confs SET deleted = $time() WHERE cnum=$inc(1)
set h  $sql_query_open ($(query))
eval   $sql_query_close($(h))
if $less ($(h) 0)
   set result <FONT COLOR=RED>Error deleting conference number $inc(1).</font>
end

