
/*** UNIT_WIPE.  Clear specific types of UNIT I/O.
/
/    Unit_wipe removes all locks of a particular TYPE established
/    by unit_lock().  Unit_wipe is generally called immediately
/    after leaving a conference, or on exiting Caucus.
/
/    Generally, there should be no need to call unit_wipe(),
/    in that every unit_lock() has it's corresponding unit_unlk().
/    However, some systems, such as Primos, leave guardian lock
/    files lying around that ought to be cleaned up.  Unit_wipe
/    also does this cleanup.
/
/    TYPE must be one of:
/       LCNF  --  clean up locks associated with a conference
/       LUSR  --  clean up locks associated with a user
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:55 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "handicap.h"
#include "systype.h"
#include "units.h"

extern short confnum;

FUNCTION  unit_wipe (type)
   int    type;
{

/*-------------------------------UNIX-------------------------*/
   extern struct locktype_t locktype[];

   ENTRY ("unitwipe", "");

   if (locktype[type].fd >= 0) {

#if UNIX | NUT40
      close (locktype[type].fd);
      locktype[type].fd = -1;
   /* Infamous corrupted masteres file */
/*    unlink (locktype[type].file); */
#endif

#if WNT40
      CloseHandle (locktype[type].handle);
#endif

      locktype[type].nconf = locktype[type].user[0] = 0;
   }
   RETURN (1);
}
