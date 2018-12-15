/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** NLADD.   Add STR and N to namelist P.  Keep it sorted. */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: JV  3/17/90 19:51 Removed stdio.h */
/*: CR  4/12/90 15:11 Rename strcomp() -> sysstrcomp(). */
/*: DE  4/14/92 15:07 Chixified */
/*: CR 12/11/92 17:40 Cast chxlen() down to int. */
/*: CR 12/17/92 12:22 Simplify; we don't really need chxlen call. */

#include "caucus.h"

extern union null_t null;

FUNCTION  nladd (p, str, n)
   struct namlist_t *p;
   int    n;
   Chix   str;
{
   struct namlist_t *new, *nlnode();

   ENTRY ("nladd", "");

   if (p == null.nl)  RETURN(0);

   if ( (new = nlnode(40)) == null.nl)  RETURN (0);
   chxcpy (new->str, str);
   new->num = n;

   for (;   p->next != null.nl;   p=p->next) 
      if (chxcompare (str, (p->next)->str, ALLCHARS) < 0)  break;

   new->next = p->next;
   p->next   = new;
   RETURN (1);
}
