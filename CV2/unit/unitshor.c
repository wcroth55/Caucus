
/*** UNIT_SHORT.  Reads NUMBER short ints into ARRAY from file N.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:55 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <ctype.h>
#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

#if UNIX | NUT40 | WNT40

FUNCTION  unit_short (n, array, number)
   int   n, number;
   short array[];
{
   Chix new;
   char newstr[80];
   int  success;

   ENTRY ("unitshort", "");
   new        = chxalloc (L(80), THIN, "unitshort new");
   success    = 0;

   if (NOT unit_read (n, new, L(0)))
      FAIL;
   ascofchx (newstr, new, L(0), L(80));
   strshort (array, newstr, number); 
   SUCCEED;

 done:

   chxfree ( new );

   RETURN ( success );
}
#endif
