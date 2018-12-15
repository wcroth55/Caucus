#
#---MAN_FULLGROUP.I    Calculate *full* list of people in a group,
#                      via SQL.
#
#   inc(1)  group name
#
#   output: variable 'list' of userids (sorted by userid), and
#             'uniq'ed (duplicates removed)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 07/23/03 New script.
#-------------------------------------------------------------------

set sql SELECT distinct userid from groups \
         WHERE name = '$inc(1)'

set list
for row in $sql_select ($(sql))
   set col  $sql_row ($(row))
   set list $(list) $sql_col ($(col))
end
