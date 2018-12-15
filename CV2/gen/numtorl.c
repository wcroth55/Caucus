
/*** NUM_TO_RL.  Add a string of a numeric range onto an Rlist.
/
/    newptr = num_to_rl (oldptr, range, first);
/
/    Parameters:
/       Rlist  newptr;      (pointer to last used node in Rlist)
/       Rlist  oldptr;      (head of Rlist we're adding to)
/       Chix   range;       (range to be parsed)
/       int    first;       (# of 1st response if unspecified item)
/
/    Purpose:
/       Parse a range of numbers (e.g. "1", "5, 6", "7-13") and add
/       them onto an Rlist of items.
/
/    How it works:
/
/    Returns:  pointer to last accessed ("added to") node in Rlist
/       OLDPTR.
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
/    Home:  gen/numtorl.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  6/14/93 10:24 New function. */
/*: CP  6/20/93 18:00 Change args. */

#include <stdio.h>
#include "caucus.h"
#include "api.h"

FUNCTION  Rlist  num_to_rl (oldptr, range, first)
   Rlist  oldptr;
   Chix   range;
   int    first;
{
   int    item1, item2;
   int4   value, pos;
   Rlist  this, add_to_rlist();
   int    can_add;

   ENTRY ("num_to_rl", "");

   this = oldptr;
   can_add = 0;

   for (pos=0;   value = chxvalue (range, pos);   ) {
      if      (cf_digit ((int) value)) {
         item1 = chxint4 (range, &pos);
         this  = add_to_rlist (this, item1, first, -1);
         can_add = 1;
         continue;
      }

      if (value == '-') {
         ++pos;
         item1 = max (item1, 1);
         item2 = chxint4 (range, &pos);
         if (can_add)  this->i1 = item2;
      }
      else if (value == ' ')  ++pos;
      else if (value == ',')  ++pos;
      else                    break;
      can_add = 0;
   }

   RETURN (this);
}
