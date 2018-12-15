
/*** CHXLEN.  Compute length of a chix, in characters.
/
/    i = chxlen (x);
/
/    Parameters:
/       int4  i;
/       Chix  x;
/
/    Purpose:
/       Count up and return the number of characters (not bytes) in
/       chix X.
/
/    How it works:
/       Scans each character in X until it finds one that has *all*
/       bytes equal to 0.  (If it doesn't find such a zero, force one
/       at the maximum length of the string, and complain.)
/
/    Returns: n >= 0 for a valid chix
/                  0 for a nullchix
/                 -1 on error
/
/    Error Conditions:  invalid chix (also calls chxerror to handle errors)
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  chx...() functions, and application.
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Speed: 1-byte, replace pointer math with strlen() ?
/
/    Home:  chx/chxlen.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR  7/23/91 20:06 Allow X to be a nullchix. */
/*: CR 10/15/91 18:51 Set actlen on forced termination. */
/*: CR 12/10/92 14:57 Change return value to long. */
/*: OQ 03/01/98 replace call to chxcheck by inline commands */
#include <stdio.h>
#include "chixdef.h"

FUNCTION  int4 chxlen (x)
   Chix   x;
{
   int4   i, j, n;
   int    sum;
   uchar *p;

   ENTRY ("chxlen", "");

   if (x == nullchix)                  RETURN ( 0);

/* if (NOT chxcheck (x, "chxlen"))     RETURN (-1); */

   if (FREED(x))    
      { chxerror (CXE_FREED, "chxlen", x->name);   RETURN(-1); }
   if (NOT ISCHIX(x))
      { chxerror (CXE_NOTCHX,"chxlen", "");        RETURN(-1); }

   /*** If we already know the length, return it. */
   if (x->actlen >= 0)                 RETURN (x->actlen);

#if USE_THIN_CHIX
/*
   for (i=n=0;   n < x->maxlen;   i++, n++) {
      sum=0; sum |= x->p[i];
      if  (sum==0)  { x->actlen = n;   RETURN (n); }
   }
*/

   for (p=x->p;   *p;   ++p) ;
   n = p - x->p;
   if (n < x->maxlen)  RETURN (x->actlen = n);

   /*** If we got here, there was no 0 termination!  Complain, and
   /    force a termination. */
   n = x->maxlen;
   x->p[n-1] = 0;

#else
   /*** For each character, add up all the bytes.  If they sum to
   /    0, we've hit the end of the string. */
   for (i=n=0;   n < x->maxlen;   i += x->type, n++) {
      for (j=sum=0;   j < x->type;   ++j)  sum |= x->p[i+j];
      if  (sum==0)  { x->actlen = n;   RETURN (n); }
   }

   /*** If we got here, there was no 0 termination!  Complain, and
   /    force a termination. */
   i = --n * x->type;
   for (j=0;   j < x->type;   ++j)  x->p[i+j] = 0;
#endif   

   chxerror (CXE_NOEND, "chxlen", x->name);
   x->actlen = n;
   RETURN   (n);
}
