
/*** CHXREPLACE.  Replace all instances of B in A (at/after POS) with C.
/
/    i = chxreplace (a, pos, b, c);
/
/    Parameters:
/       int   i;    (1 ==> at least one B replaced, 0 ==> B not found)
/       Chix  a;    (search in this chix...)
/       int4  pos;  (...starting at this position...)
/       Chix  b;    (...for this chix...)
/       Chix  c;    (...and where found, replace with this chix.)
/
/    Purpose:
/       Search chix A, starting at position POS, for the all occurences
/       of chix B.  Replace them with C.
/
/       Note that, as usual, B may be found in A, even if they have
/       different types, so int4 as their corresponding characters match
/       after promotion.  If B is found, and C's type > A's type, then
/       A is promoted to C's type.
/
/    How it works:
/
/    Returns: 1 if at least one instance of B found and replaced
/             0 if no instances of B are found.
/             0 if A or B are empty strings.
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
/    Speed: new 1-byte case, replace chxvalue and n-byte loops
/
/    Home:  chx/chxalter.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/11/96 14:00 New function, based on chxalter. */
/*: OQ  2/16/98 replace chxcheck by in line codes */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxreplace (Chix a, int4 pos, Chix b, Chix c)
{
   int4          cval, a_end, b_end;
   Chix          z;
   struct chix_t swap;
   uchar        *p;

   ENTRY ("chxreplace", "");

   /*** Input validation. */
   /*if (NOT chxcheck (a, "chxreplace")  ||  NOT chxcheck (b, "chxreplace")  ||
       NOT chxcheck (c, "chxreplace")  ||
       chxvalue (a, 0L) == 0L          ||  chxvalue (b, 0L) == 0L)   RETURN (0);*/
   if ( a == nullchix || b == nullchix  || c== nullchix)
      { chxerror (CXE_NULL, "chxreplace", "");                RETURN(0); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxreplace", a->name);          RETURN(0); }
   if (FREED(b))    
      { chxerror (CXE_FREED, "chxreplace", b->name);          RETURN(0); }
   if (FREED(c))    
      { chxerror (CXE_FREED, "chxreplace", c->name);          RETURN(0); }
   if (NOT ISCHIX(a) || NOT ISCHIX(b) || NOT ISCHIX(c))
      { chxerror (CXE_NOTCHX, "chxreplace", "");              RETURN(0); }
   if ( chxvalue (a, 0L) == 0L  ||  chxvalue (b, 0L) == 0L)   RETURN (0);

   /*** Can we find B in A? */
   if ( (pos = chxindex (a, pos, b)) < 0)   RETURN (0);

   /*** Make sure type of A is at least the size of the type of C. */
   if (c->type > a->type)  chxresize (a, c->type, 0L);

   /*** Build a new chix Z, out of the first part of A 
   /    (before the 1st instance of B). */
   z = chxalloc ((int4) a->maxlen, a->type, a->name);
   chxcatsub (z, a, 0L, pos);

   
   /*** Scan the rest of A character by character.  If it starts
   /    an instance of B, add C to Z.  Otherwise add the char to Z. */
   for (  ;   (cval = chxvalue (a, pos));   ++pos) {
      p = a->p + pos  * a->type;
      chxscan (p, a->type,   b->p, b->type,   &a_end, &b_end);

      if (b_end != 0L)  {
         chxcatval (z, a->maxlen, cval);
         continue;
      }

      chxcat (z, c);
      pos += chxlen(b) - 1;
   }

   /*** Exchange Z and A. */
   swap = *a;
  *a    = *z;
   *z   = swap;
   
   /*** Free the old A, now called Z. */
   chxfree (z);

   RETURN  (1);
}
