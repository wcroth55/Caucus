
/*** TOUCH_MASTER.  Bring the old master index up to date with
/    the current master index. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:52 Source code master starting point */
#include "caucus.h"

extern struct master_template master, oldmaster;

FUNCTION  touch_master()
{
   int    i;

   ENTRY ("touchmaster", "");

   for (i=0;   i<=master.items;   ++i)
      oldmaster.responses[i] = master.responses[i];

   oldmaster.items = master.items;
   RETURN (1);
}
