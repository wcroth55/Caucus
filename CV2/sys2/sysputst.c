/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** SYSPUTSTR.  Display the characters in STR on the user's terminal. */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

FUNCTION  sysputstr (str)
   char *str;
{

/*--------------------------FORTUNE & VAX & MASSCOMP & A3---------------*/
#if UNIX | NUT40 | WNT40
   ENTRY ("sysputstr", "");
   if (str == NULL)  RETURN (1);

   fputs  (str, stdout);

   fflush (stdout);
#endif


   RETURN (1);
}
