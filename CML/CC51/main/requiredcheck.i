#
#---requiredcheck.i    Check for all required personal info fields.
#
#   Output:  variable regerrs.  Empty if all required fields have
#            data, otherwise contains text of error messages.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  9/02/04 Use %urcregnums() to get REG field numbers.
#: CR 12/19/05 Completely rewrite to use user_iface 'required' field.
#-----------------------------------------------------------------

set query SELECT name, label FROM user_iface WHERE iface='$(iface)' \
             AND required=1
set h $sql_query_open($(query))
while $sql_query_row ($(h))
   if $not_equal ($str_sub (0 1 $(name)) {)
      set q SELECT $(name) FROM user_info WHERE userid='$userid()'
      set fieldcheck $sql_query_select ($(q))
      if $empty ($(fieldcheck))
         set regerrs $(regerrs) <p/>Please enter a value for '<b>$(label)</b>'.
      end
   end
end
eval  $sql_query_close($(h))
#-----------------------------------------------------------------
