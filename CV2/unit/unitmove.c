
/*** UNIT_MOVE.  Moves (renames) file number A to file number B. 
/
/    Both files A and B must have been locked by UNIT_LOCK.
/    Returns 1 if successful, and 0 otherwise. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:55 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include "caucus.h"
#include <stdio.h>
#include "systype.h"
#include "units.h"

extern struct unit_template units[];
extern int                  debug;

FUNCTION  unit_move (a, b)
   int    a, b;
{
   char  *bugtell();

#if UNIX | NUT40 | WNT40
   int   success, local;

   ENTRY  ("unitmove", "");
   local = (a < XURG  &&  b < XURG);
   if (local)  sysprv(0);
   success = sysrename (units[a].nameis, units[b].nameis);
   if (local)  sysprv(1);
   if (debug) {
      char  both[120];
      sprintf (both, "%s -> %s", units[a].nameis, units[b].nameis);
      bugtell ("MOVE", success, (int4) (a*1000L + b), both);
   }
   RETURN (success);
#endif
}
