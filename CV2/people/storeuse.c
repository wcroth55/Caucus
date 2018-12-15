
/*** STORE_USER.  Rewrites the user participation file for this conference.
/    Assumes the XUPA file is already locked. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: JV  3/27/90 12:51 Removed unnecessary delacration of screen. */
/*: DE  4/29/92 12:20 Chixified */
/*: JX  6/06/92 19:53 Add FORMAT arg to sysdaytime(). */
/*: CR 12/21/92 14:32 Speed up function by using sprintf(). */
/*: CB  6/01/93 16:48 Add <stdio.h> */
/*: CR  8/20/04 Add unit argument.  */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  store_user (struct partic_t   *thisconf, int unit) {
   short  i;
   Chix   day, time, daytime;
   char   temp[40];
   int4   systime();

   ENTRY ("storeuser", "");

   if (NOT unit_make (unit))  RETURN (0);

   day      = chxalloc (L(30),  THIN, "storeuse day");
   time     = chxalloc (L(30),  THIN, "storeuse time");
   daytime  = chxalloc (L(60),  THIN, "storeuse daytime");

   sysdaytime (day, time, 0, systime());
   chxformat  (daytime, CQ("%s %s\n"), L(0), L(0), day, time);
   unit_write (unit, daytime);

   sprintf    (temp, "%d\n", thisconf->items);
   unit_write (unit, CQ(temp));

   for (i=1;  i<=thisconf->items;  ++i) {
      sprintf (temp, "%d %d\n", thisconf->responses[i], thisconf->backresp[i]);
      unit_write (unit, CQ(temp));
   }

   unit_close(unit);

   chxfree ( day );
   chxfree ( time );
   chxfree ( daytime );

   RETURN (1);
}
