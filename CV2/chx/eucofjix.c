
/*** EUCOFJIX.  Convert (part of) a jix to EUC, put result in ASCII string.
/
/    eucofjix (s, a, i, n);
/
/    Parameters:
/       char *s;    (Where to place the ascii string.)
/       Chix  a;    (Jix to be converted.)
/       int4  i;    (Start at character position I in A.)
/       int4  n;    (Convert as much as will fit in N bytes.)
/
/    Purpose:
/       Convert (a substring of) a jix A of japanese characters into a 
/       stream of bytes in EUC format, in S.
/
/    How it works:
/       Start at character position I in A.  Convert as much of A as will
/       fit into N bytes of S (including placing a terminating null byte
/       into S.)   If I < 0, uses I=0.
/       
/    Returns:
/       Number of characters from A that were converted.
/       0 if *all* of A (from position I on) was converted.
/      -1 on error.
/
/    Error Conditions:
/       S is NULL.
/       A is not chix (sets S to the empty string).
/       N <= 0                    (ditto)
/       I too large.              (ditto)
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  chx/eucofjix.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/24/91 14:45 New function. */
/*: CX  6/08/92 10:52 Allow A to be THIN chix. */
/*: CR 12/11/92 15:02 Args I and N are long. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  eucofjix (s, a, i, n)
   char  *s;
   Chix   a;
   int4   i, n;
{
   int    more0, more1, x, conv;
   int4   j;

   ENTRY ("eucofjix", "");

   /*** Input validation. */
   if (s == NULL)                     RETURN (-1);
   *s = '\0';
   if (NOT chxcheck (a, "eucofjix"))  RETURN (-1);
   if (i >= a->maxlen)                RETURN (-1);
   if (i < 0)  i = 0;
   
   /*** Do single-byte chix case; it's easy. */
   if (a->type == THIN) {
      for (j=0;    a->p[i+j]  &&  j<n-1;   ++j)  s[j] = a->p[i+j];
      s[j] = '\0';
      RETURN (a->p[i+j] ? j : 0);
   }

   /*** Convert the 2-byte jix chars from A into stream of bytes in S.
   /    I is the starting character position in A.
   /    X is the current byte position in A.
   /    CONV counts how many of A's characters have been converted.
   /    J is the index into S. */
   x = 2 * i;
   for (conv=0, j=0, more0=more1=1;    j<n-2  &&  (more0 || more1);
                                       x+=2, ++conv) {
      more0 = 0;
      if (a->p[x] != 0)   s[j++] = more0 = a->p[x];
      s[j++] = more1 = a->p[x+1];
   }
   s[j] = '\0';
      
   RETURN (more0 || more1 ? conv : 0);
}
