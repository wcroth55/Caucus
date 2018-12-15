
/*** UNIT_CHECK.  Does a file exist?
/
/    Assumes that the caller has already locked the file of type N.
/    Returns 1 if that file exists, and 0 otherwise. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:54 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

extern struct unit_template units[];

FUNCTION  unit_check (n)
   int  n;
{

#if UNIX | NUT40 | WNT40
   int  exists;

   ENTRY  ("unitcheck", "");
   if (n < XURG)  sysprv(0);
   exists = syscheck (units[n].nameis);
   if (n < XURG)  sysprv(1);
   RETURN (exists);
#endif

}
