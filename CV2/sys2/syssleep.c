/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSSLEEP.   Pause for approximately N seconds. */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#if WNT40
#include <windows.h>
#include <stdlib.h>
#include <iostream.h>
#endif

FUNCTION  syssleep (n)
   int    n;
{
   ENTRY ("syssleep", "");

#if UNIX | NUT40
   sleep ( (unsigned) n);
#endif

#if WNT40
   Sleep ( 1000 * n);
#endif

   RETURN (1)
}
