
/*** SYSFLUSHIN.   Flush all outstanding input from the user's terminal. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

FUNCTION  sysflushin()
{

#if SUN41
#include <sgtty.h>
   int  ready;
   char str[82];

   ENTRY ("sysflushin", "");

   ready = 0;
   while ( ioctl (0, FIONREAD, &ready) != -1  &&  ready > 0 ) {
      read  (0, str, (ready > 80 ? 80 : ready));
   }

   RETURN (1);
#endif
}
