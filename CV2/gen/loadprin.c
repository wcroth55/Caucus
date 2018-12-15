
/*** LOADPRINT.   Load the print codes from file U.
/
/    Loads printcode instructions from file U into the THISUSER.PRINT
/    namelist.  If LOCK is true, LOADPRINT locks and unlocks file U.
/    Otherwise, the caller must handle the locking and unlocking. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: DE  3/31/92 14:51 Chixified */
/*: DE  6/02/92 11:12 Added maxlen arg to unit_read */

#include "caucus.h"

extern union null_t       null;

FUNCTION  loadprint  (print, u, lock)
   struct namlist_t **print;
   int    u, lock;
{
   struct namlist_t *nlnode();
   Chix   str;
   int    success;

   ENTRY ("loadprint", "");

   str      = chxalloc (L(160), THIN, "loadprin str");
   success  = 0;

   nlfree (*print);
   *print = nlnode (4);

   if (lock  &&  NOT unit_lock (u, READ, L(0), L(0), L(0), L(0), null.chx)) FAIL;

   if (NOT unit_view (u)) {
      if (lock)  unit_unlk (u);
      FAIL;
   }

   while (unit_read (u, str, L(0)))  nlcat (*print, str, 0);

   unit_close (u);
   if (lock)  unit_unlk (u);
   SUCCEED;

 done:

   chxfree ( str );

   RETURN ( success );   
}
