#
#---statwizhead.i   Add a header cell to stat wizard output
#
#   inc(1)   Number of cells to use
#   inc(2)   Cell contents
#            (if empty, output header row)
#   inc(3)   other <td> cell fields
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 07/26/03 New script.
#: CR 04/25/05 Add new inc(1).
#--------------------------------------------------------------------

if $equal ($form(restype) w)
   if $empty ($(statwizheadNew))
      "<tr valign=bottom>
      set statwizheadNew 1
   end
   if $empty ($inc(1))
      set statwizheadNew
   end
   if $not_empty ($inc(1))
      "<td colspan="$inc(1)" $inc(3)>&nbsp;<b>$inc(2)</b></td>
   end
end

else
   if $empty ($inc(1))
      eval $writeln ($(fd) $unquote($(spreadsheet)))
      set spreadsheet
   end
   else
      set tabs $str_sub (0 $inc(1) xxxxxxxxxxxxxxxxxxxx)
      set spreadsheet $quote($unquote($(spreadsheet))\
                             $inc(2)$replace(x 09 $(tabs)))
   end
end
