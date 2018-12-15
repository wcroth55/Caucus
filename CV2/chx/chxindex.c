/*** CHXINDEX.  Find position of chix Y in chix X, starting at position POS.
/
/    n = chxindex (x, pos, y);
/
/    Parameters:
/       int4  n;     (character position in X where Y starts)
/       Chix  x;     (search in this chix...)
/       int4  pos;   (...starting at this position...)
/       Chix  y;     (...for this chix.)
/
/    Purpose:
/       Search chix X, starting at position POS, for the first occurence of 
/       chix Y.  Return the number of the character position in X where Y 
/       starts.  (Note that, as usual, Y may be found in X, even if they have
/       different types, so int4 as their corresponding characters match
/       after promotion.)
/
/    How it works:
/       Starting at POS, for each character in X, see if that character begins
/       a match with Y.  Call chxscan() to check for the match.
/
/    Returns: N, number of character position in X where Y starts.
/       The first character of X is number 0.
/
/       Returns -1 if:  Y is not found in X,
/                       Y or X are empty.
/                       Y or X are not chix.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  application
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Speed: 1-byte, replace double counter with pointer math
/
/    Home:  chx/chxindex.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR 10/18/91 14:54 Add POS argument. */
/*: CR 12/10/92 14:56 Change POS & return value to long. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: OQ 02/09/98 replace chxcheck by inline commands */
/*: OQ 02/28/98 added code for single byte character chix type */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  int4 chxindex (x, pos, y)
   Chix   x, y;
   int4   pos;
{
   int4   x_end, y_end, sum;
   int4   i, j;
   uchar *p;

   ENTRY ("chxindex", "");

   /*** Input validation. */
   /*if (NOT chxcheck (x, "chxindex"))   RETURN (-1);*/
   /*if (NOT chxcheck (y, "chxindex"))   RETURN (-1);*/
   if (x == nullchix || y == nullchix )
      { chxerror (CXE_NULL, "chxindex" , "");        RETURN(-1); }
   if (FREED(x))    
      { chxerror (CXE_FREED, "chxindex" , x->name);   RETURN(-1); }
   if (FREED(y))    
      { chxerror (CXE_FREED,  "chxindex" , y->name);   RETURN(-1); }
   if ( (NOT ISCHIX(x)) || (NOT ISCHIX(y)) )
      { chxerror (CXE_NOTCHX, "chxindex" , "");        RETURN(-1); }

   if (chxvalue (y, 0L) == 0L)         RETURN (-1);
   if (x->actlen < 0)  chxlen (x);
   if (pos >= x->actlen)               RETURN (-1);

   /*** Starting at POS, examine each character in X.  Does it start a
   /    match with Y? */
#if USE_THIN_CHIX
   for (i=pos, p = x->p + pos;   (1);    ++i, p++) {

      /*** Are we at the end of X? */
      if (NOT *p)   break;

      chxscan (p, x->type, y->p, y->type, &x_end, &y_end);
      if (y_end == 0L)  RETURN (i);
   } 

#else
   for (i=pos, p = x->p + pos * x->type;    (1);    ++i, p += x->type) {

      /*** Are we at the end of X? */
      for (sum=0L, j=0;   j<x->type;   ++j)  sum |= p[j];
      if  (sum == 0L)  break;

      chxscan (p, x->type, y->p, y->type, &x_end, &y_end);
      if (y_end == 0L)  RETURN (i);
   }
#endif

   RETURN (-1);
}
