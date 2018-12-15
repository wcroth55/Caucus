
/*** UNIT_KILL.   Kill (delete) a given file.
/
/   n = unit_kill (unit);
/
/   Parameters:
/      int unit;
/
/   Purpose:
/      Delete the file currently locked by UNIT_LOCK of type UNIT.
/
/   How it works:
/      On most systems, it calls sysunlink() on the filename
/      associated with UNIT.  On server-based systems, it calls
/      sysunlink for local files, but for Caucus files it sends
/      a message to the server.
/
/   Note: the file must be locked before calling unit_kill, and should
/      be unlocked afterward.  The file should not be opened for
/      reading or writing.
/
/   Return:
/      Most systems always return 1.
/      Server-based systems return 1 on success, 0 on failure.
/
/   Related functions: unit_lock, unit_unlk.
/
/   Called by: lots and lots.
/
/   Operating system dependencies: See above.
/
/   Home: unit/unitkill.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:54 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include "caucus.h"
#include <stdio.h>
#include "systype.h"
#include "units.h"

extern struct unit_template units[];
extern int                  debug;

FUNCTION  unit_kill (n)
   int  n;
{
   char *bugtell();

#if UNIX | NUT40 | WNT40
   ENTRY ("unitkill", "");
   if (debug & DEB_FILE)  bugtell ("KILL", n, L(0), units[n].nameis);
   if (n < XURG)  sysprv(0);
   sysunlink (units[n].nameis);
   if (n < XURG)  sysprv(1);
   RETURN (1);
#endif

}
