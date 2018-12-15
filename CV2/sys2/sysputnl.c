/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSPUTNL.  Put a "newline" out on the user's terminal. */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

FUNCTION  sysputnl()
{
   ENTRY ("sysputnl", "");

/*--------------------------"Normal" computers---------------------*/
#if UNIX | NUT40 | WNT40
   sysputstr ("\n");
#endif

   RETURN (1);
}
