/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** NLCAT.   Add STR and N to the end of namelist P. */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: JV  3/17/90 19:51 Removed stdio.h */
/*: DE  4/14/92 15:11 Chixified */
/*: CR 12/11/92 17:40 Cast chxlen() down to int for nlnode. */

#include "caucus.h"

extern union null_t null;

FUNCTION  nlcat (p, str, n)
   struct namlist_t *p;
   int    n;
   Chix   str;
{
   struct namlist_t *new, *nlnode();

   ENTRY ("nladd", "");

   if (p == null.nl)  RETURN(0);

   if ( (new = nlnode ( (int) chxlen(str) )) == null.nl)  RETURN (0);
   chxcpy (new->str, str);
   new->num = n;

   for (;   p->next != null.nl;   p=p->next) ;

   p->next = new;
   RETURN (1);
}
