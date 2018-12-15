
/*** STORNMDIR.   Store name directory for file type N.
/
/    Writes the name directory for file N from NMDIR.  Assumes
/    file N is already locked.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: DE  4/29/92 12:30 Chixified */
/*: CX  6/29/92 14:23 chxclear line before each chxformat. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  stornmdir (n, nmdir)
   int    n;
   struct namlist_t   *nmdir;
{
   Chix    line;

   ENTRY ("stornmdir", "");

   if (NOT unit_make (n))  RETURN(0);

   line = chxalloc (L(80), THIN, "stornmdi line");

   for (nmdir = nmdir->next;   nmdir != null.nl;   nmdir = nmdir->next) {
     chxclear   (line);
     chxformat  (line, CQ("%5d %s\n"), L(nmdir->num), L(0), nmdir->str, null.chx);
     unit_write (n, line);
   }

   unit_close (n);

   chxfree ( line );

   RETURN(1);
}
