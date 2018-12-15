/*** PARSE_NUM.  Parse a numeric range, mark numbers selected in FLAGS.
/
/    found = parsenum (range, flags, max_items, mark);
/
/    Parameters:
/       int    found;       (were any numbers selected and marked?)
/       Chix   range;       (range to be parsed)
/       short  flags[];     (array of flags)
/       int    max_items;   (size of flags)
/       int    mark;        (set selected items to this)
/
/    Purpose:
/       Parse a range of numbers (e.g. "1", "5, 6", "7-13"), and mark
/       the equivalent entries in FLAGS.
/
/    How it works:
/       For each item I found in the RANGE, set FLAGS[I] to MARK.
/
/    Returns:  1 if at least one number N was found in RANGE.
/              0 otherwise
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  gen/parsenum.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: CR 10/22/91 14:11 Add max_items argument. */
/*: CX  5/27/92 12:39 Guru-ized. */
/*: JX  5/27/92 15:22 Fixify. */
/*: JX  6/19/92 12:37 Don't SUCCEED at end. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR  5/12/93 22:02 Remove 'and' argument, add 'mark' arg. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  parsenum (range, flags, max_items, mark)
   Chix   range;
   short  flags[];
   int    max_items, mark;
{
   int    success;
   int    item1, item2, i;
   int4   value, pos;

   ENTRY ("parsenum", "");

   success = 0;

   for (pos=0;   value = chxvalue (range, pos);   ) {
      if      (cf_digit ((int) value)) {
         item1 = chxint4 (range, &pos);
         if (item1>0  &&  item1<=max_items) {
            flags[item1] = mark;
            success = 1;
         }
      }
      else if (value == '-') {
         ++pos;
         item1 = max (item1, 1);
         item2 = min ( (int) chxint4 (range, &pos), max_items);
         for (i=item1;  i<=item2;  ++i) {
            flags[i] = mark;
            success  = 1;
         }
      }
      else if (value == ' ')  ++pos;
      else if (value == ',')  ++pos;
      else                    break;
   }

   RETURN (success);
}
