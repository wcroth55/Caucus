/*** CHXINSIDE.   Find a pattern inside two boundary strings.
/
/    Search CHXA for the substring bounded by the one-character strings
/    LEFT and RITE.  Return the substring in MATCH.  If the boundaries
/    were found, return the position at which LEFT was found.  If the
/    boundaries were not found, return -1. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  5/19/92 13:28 Create function. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */

#include "caucus.h"

FUNCTION chxinside (chxa, left, match, rite)
   Chix  chxa, left, match, rite;
{
   int    i, j, clen;
   int4   leftval, riteval, charj;

   ENTRY ("chxinside", "");

   leftval = chxvalue (left, L(0));
   riteval = chxvalue (rite, L(0));
   clen    = chxlen (chxa);

   /*** Find the first occurence of LEFT. */
   for (i=0; chxvalue (chxa, L(i)) != leftval && i<clen; i++) ;
   if  (i >= clen)  RETURN (-1);

   chxclear (match);
   /*** Scan until RITE is found, copying into MATCH all the while. */
   for (j=i+1;  chxvalue (chxa, L(j)) != riteval && j<clen;   j++) {
      charj = chxvalue (chxa, L(j));
      chxcatval (match, jixwidth (charj), charj);
   }

   if    (j >= clen)  RETURN (-1)
   else               RETURN (i)
}
