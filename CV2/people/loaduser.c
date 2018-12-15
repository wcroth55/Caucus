
/*** LOAD_USER.  Load the user participation file for the given conference.
/
/    LOAD_USER returns:
/      1 if the user's file was successfully loaded;
/      0 if the user's file does not exist (new participant)  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: JV  3/27/90 12:48 Removed unnecessary declaration of screen & mtu. */
/*: DE  6/01/92 17:00 Patched unit_read */
/*: CR  8/20/04 Added 'unit' argument.  */

#include "caucus.h"

FUNCTION  load_user (struct partic_t  *thisconf, int unit) {
   short  i, list[80];

   ENTRY ("load_user", "");

   if (NOT unit_view (unit))  RETURN (0);

   unit_read (unit, thisconf->lastin, L(0));
   unit_short(unit, &thisconf->items, 1);
   for (i=0;  i<MAXITEMS;     ++i) {
      thisconf->responses[i] = -1;
      thisconf->backresp [i] =  0;
   }

   for (i=1;  i<=thisconf->items;  ++i)  {
      unit_short (unit, list, 2);
      thisconf->responses[i] = list[0];
      thisconf->backresp [i] = list[1];
   }
   unit_close(unit);

   RETURN (1);
}
