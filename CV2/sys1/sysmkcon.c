
/*** SYSMKCONF.  Make (create) a conference directory.
/
/    Make the conference directory for conference number N.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include "caucus.h"
#include "systype.h"
#include <stdio.h>

/*ARGSUSED*/

extern union null_t null;

FUNCTION  sysmkconf (n)
   int    n;
{

#if UNIX | NUT40 | WNT40
   char   name[80];

   ENTRY ("sysmkcon", "");

   unit_name (name, XCDD, L(n), L(0), L(0), L(0), null.chx);
   sysmkdir  (name, 0700, NULL);
   RETURN (1);
#endif

}
