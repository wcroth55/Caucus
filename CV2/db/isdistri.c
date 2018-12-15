
/*** IS_DISTRIBUTED.  Is this conference distributed?
/
/    yes = is_distributed (cnum);
/
/    Parameters:
/       int    yes;     (1 if conference CNUM is distributed, else 0)
/       int    cnum;    (number of conference in question)
/
/    Purpose:
/       Find out if changes & additions to this conference should be
/       distributed to (at least one) other host(s).
/
/    How it works:
/       Check to see if this Caucus license allows distributed.  Then
/       check to see if there is a "neighbors" file for this conference.
/       Save the conf number and result of this call for next call.
/
/    Returns: 1 if true, else 0
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  db/isdistri.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/30/91 13:34 New function. */
/*: JV 12/10/91 16:04 Cache data. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JX  5/21/92 14:11 Chixify. */
/*: CR  1/26/93 15:49 Use PLAIN and strplain() to decrypt license info. */
/*: CR  5/19/94 12:33 Return 1 only if XCNB is not empty. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  is_distributed (cnum)
   int    cnum;
{
   static int   yes, cache_num=0;
   Chix         temp;

   ENTRY ("is_distributed", "");

   RETURN (0);

   if (cnum && cnum == cache_num)                             RETURN (yes);

   yes = 0;
   if (unit_lock (XCNB, READ, L(cnum), L(0), L(0), L(0), nullchix)) {
      if (unit_view (XCNB)) {
         temp = chxalloc  (L(80), THIN, "is_distri temp");
         yes  = unit_read (XCNB, temp, L(0));
         chxfree (temp);
         unit_close (XCNB);
      }
      unit_unlk (XCNB);
   }
   cache_num = cnum;
   RETURN (yes);
}
