/*** CHXREVDEX.  Reverse search: find chix Y in chix X, starting at POS.
/
/    n = chxrevdex (x, pos, y);
/
/    Parameters:
/       int4  n;     (character position in X where Y starts)
/       Chix  x;     (search in this chix...)
/       int4  pos;   (...starting at this position...)
/       Chix  y;     (...for this chix.)
/
/    Purpose:
/       Search *backwards* through chix X, starting at position POS, for
/       the "first" (last?) occurence of chix Y.  Return the number of 
/       the character position in X where Y starts.  (Note that, as usual,
/       Y may be found in X, even if they have different types, so long
/       as their corresponding characters match after promotion.)
/
/    How it works:
/       If POS > length of X, search starting from the end of X.
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
/    Home:  chx/chxrevdex.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/26/92 13:41 New function. */
/*: CR 12/11/92 13:26 POS, return value now long. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: OQ  2/17/98 replace chxcheck by in line code */
/*: OQ  3/02/98 added code for single-byte character chix type */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  int4 chxrevdex (x, pos, y)
   Chix   x, y;
   int4   pos;
{
   int4   x_end, y_end;
   int4   i;
   uchar *p;

   ENTRY ("chxrevdex", "");

   /*** Input validation. */
   /*if (NOT chxcheck (x, "chxrevdex"))   RETURN (-1);*/
   /*if (NOT chxcheck (y, "chxrevdex"))   RETURN (-1);*/
   if ( x == nullchix || y == nullchix )
      { chxerror (CXE_NULL, "chxrevdex", "");        RETURN(-1); }
   if (FREED(x))    
      { chxerror (CXE_FREED, "chxrevdex", x->name);  RETURN(-1); }
   if (FREED(y))    
      { chxerror (CXE_FREED, "chxrevdex", y->name);  RETURN(-1); }
   if (NOT ISCHIX(x) || NOT ISCHIX(y))
      { chxerror (CXE_NOTCHX, "chxrevdex", "");      RETURN(-1); }

   if (chxvalue (y, 0L) == 0L)                       RETURN (-1);
   if (x->actlen < 0)  chxlen (x);
   if (pos == ALLCHARS  ||
       pos >= x->actlen)                pos = x->actlen - 1;

   /*** Starting at POS and working backwards, examine each character in X.
   /    Does it start a match with Y? */
#if USE_THIN_CHIX
   for (i=pos, p = x->p + pos;    i>=0;    --i, p--) {
      chxscan (p, x->type, y->p, y->type, &x_end, &y_end);
      if (y_end == 0L)  RETURN (i);
   }

#else
   for (i=pos, p = x->p + pos * x->type;    i>=0;    --i, p -= x->type) {
      chxscan (p, x->type, y->p, y->type, &x_end, &y_end);
      if (y_end == 0L)  RETURN (i);
   }
#endif

   RETURN (-1);
}
