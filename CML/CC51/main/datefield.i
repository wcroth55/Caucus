#
#---datefield.i    Display a three-part (day/month/year) date form element
#
#   inc(1) default SELECTED time in epoch seconds
#   inc(2) base name of each field 
#          (e.g. "x" produces fields xday, xmonth, xyear)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR  7/18/03 New include file.
#:CR 08/26/03 Force year selection to go through current year + 2.
#:CR 12/20/03 Use %datetz() to display proper time zone info.
#:CR 10/08/04 Take out %datetz(), it messes up parsing.
#------------------------------------------------------------------------

#---Get the date in pieces; use it to mark that date as SELECTED in
#   the date pull-down menus.

set tdate $lower ($replace (- 32 $dateof ($inc(1))))
set tday  $plus (0 $word (1 $(tdate)))
set tmon  $upper1 ($word (2 $(tdate)))
set tyear          $word (3 $(tdate))

"<TD><SELECT NAME="$inc(2)day">
     count mday 1 31
        set m0day $str_sub (2 2 $plus(1000 $(mday)))
"       <OPTION VALUE=$(m0day) $if ($equal ($(mday) $(tday)) SELECTED)>\
                      $(m0day)</OPTION>
     end
"    </SELECT></TD>
"<TD><SELECT NAME="$inc(2)month">
     for month in Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
"       <OPTION VALUE=$(month) $if ($equal ($(month) $(tmon)) SELECTED)>\
                      $(month)</OPTION>
     end
"    </SELECT></TD>
"<TD><SELECT NAME="$inc(2)year">
     set hi_year $plus ($word (3 $replace (- 32 %datetz($dateof ($time())))) 2)
     count year 2000 $(hi_year)
"       <OPTION VALUE=$(year) $if ($equal ($(year) $(tyear)) SELECTED)>\
                      $(year)</OPTION>
     end
"    </SELECT>&nbsp;&nbsp;&nbsp;</TD>

#------------------------------------------------------------------------
