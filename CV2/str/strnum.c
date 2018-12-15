/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** STRNUM.  Convert a string to an integer.
/
/    STRNUM tries to convert the string it finds in S to an integer.
/    If it succeeds, the result is RETURNed in I, and STRNUM RETURNs 1.
/    Otherwise, STRNUM RETURNs 0.
/
/    STRNUM understands leading blanks, and a leading minus sign. */

/*: AA  7/29/88 17:53 Source code master starting point */
/*: CR  9/14/89 15:25 Do not change I if strscan fails. */

#include "caucus.h"

FUNCTION  strnum (s, i)
   char   s[];
   int    *i;
{
   int     temp;
   char   *t, *u;
   char   minus;
   int4   strscan();

   ENTRY ("strnum", "");
   minus = 0;
   t = s;
   while (*t == ' ')    ++t;
   if    (*t == '-')  { ++t;   minus = 1; }

   u = t;
   temp = (int) strscan (&t);
   if (u == t)  RETURN (0);

   *i = temp;
   if (minus) *i = -(*i);
   RETURN (1);
}
