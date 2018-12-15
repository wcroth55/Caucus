
/*** UNIT_UNLK.   Unlock the file of type N that was locked with UNIT_LOCK. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:55 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#if UNIX | NUT40
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#if WNT40
#include <windows.h>
#include <iostream.h>
#endif

#include "units.h"

extern struct unit_template units[];
extern char                *unittag[];
extern int                  debug;
extern Chix                 confid;

FUNCTION  unit_unlk (n)
   int  n;
{
   char  *bugtell();

   extern struct locktype_t locktype[];
   extern Flag              shouldlock[];
   int    fd;

#if UNIX | NUT40
   struct flock alock = { 0, SEEK_SET, 0, 1 };
#endif

#if WNT40
   OVERLAPPED overlap;
#endif

   ENTRY ("unitunlk", "");
   if (debug & DEB_LOCK)  bugtell ("Unlk", n, L(0), (n>=0 ? unittag[n] : ""));

   if (n <= XITT)  RETURN (1);

   if (units[n].access == UNLKED)
      buglist (bugtell ("Unitunlk: ", n, L(0), "Unlocking unlocked file!\n"));
   else if (units[n].access != LOCKED)
      buglist (bugtell ("Unitunlk: ", n, L(0), "Unlocking open file!\n"));

   if (NOT shouldlock[n]  ||  units[n].locknum < 0) {
      units[n].access = UNLKED;
      RETURN (1);
   }

#if UNIX | NUT40
   fd = locktype [ units[n].locktype ].fd;
   alock.l_type   = F_UNLCK;
   alock.l_start  = units[n].locknum;
   fcntl (fd, F_SETLK, &alock);
#endif

#if WNT40
   overlap.Offset     = units[n].locknum;
   overlap.OffsetHigh = 0;
   UnlockFileEx (locktype [units[n].locktype].handle, 0,
                 1, 0, &overlap);
#endif

   units[n].locknum = -1;
   units[n].access  = UNLKED;
   RETURN (1);
}
