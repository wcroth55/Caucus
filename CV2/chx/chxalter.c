
/*** CHXALTER.  Find string B in string A at/after POS, replace with string C.
/
/    i = chxalter (a, pos, b, c);
/
/    Parameters:
/       int   i;    (1 ==> success, 0 ==> failure)
/       Chix  a;    (search in this chix...)
/       int4  pos;  (...starting at this position...)
/       Chix  b;    (...for this chix...)
/       Chix  c;    (...and if found, replace with this chix.)
/
/    Purpose:
/       Search chix A, starting at position POS, for the first occurence
/       of chix B.  Replace the instance of B with C.
/
/       Note that, as usual, B may be found in A, even if they have
/       different types, so long as their corresponding characters match
/       after promotion.  If B is found, and C's type > A's type, then
/       A is promoted to C's type.
/
/    How it works:
/
/    Returns: 1 on success (B found and replaced), 0 on failure.
/       Fails if A or B are empty strings.
/
/    Error Conditions: A, B, or C not chix.  Fails (and complains
/       via chxerror().)
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  application
/
/    Operating system dependencies:
/       Uses sysfree().
/
/    Known bugs:      none
/
/    Speed: replace chxcheck with in-line code.
/
/    Home:  chx/chxalter.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR  7/28/91 15:48 chxalloc() first argument is long. */
/*: CR 10/18/91 14:50 Add POS argument. */
/*: CR 10/22/92 18:47 A must keep its old name. */
/*: CR 12/10/92 14:54 Change POS arg to long. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CP  9/22/93 13:25 Swap A & Z, then free Z. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxalter (Chix a, int4 pos, Chix b, Chix c)
{
   int4   i;
   Chix   z;
   struct chix_t swap;

   ENTRY ("chxalter", "");

   /*** Input validation. */
   if (NOT chxcheck (a, "chxalter")  ||  NOT chxcheck (b, "chxalter")  ||
       NOT chxcheck (c, "chxalter")  ||
       chxvalue (a, 0L) == 0L        ||  chxvalue (b, 0L) == 0L)   RETURN (0);

   /*** Can we find B in A? */
   if ( (i = chxindex (a, pos, b)) < 0)   RETURN (0);

   /*** Make sure type of A is at least the size of the type of C. */
   if (c->type > a->type)  chxresize (a, c->type, 0L);

   /*** Build a new chix Z, out of the first part of A (before the instance
   /    of B), the contents of C, and the second part of A (after B). */
   z = chxalloc ((int4) a->maxlen, a->type, a->name);
   chxcatsub (z, a, 0L, i);
   chxcat    (z, c);
   chxcatsub (z, a, i + chxlen(b), a->maxlen);

   /*** Exchange Z and A. */
   swap = *a;
  *a    = *z;
   *z   = swap;
   
   /*** Free the old A, now called Z. */
   chxfree (z);

   RETURN  (1);
}
