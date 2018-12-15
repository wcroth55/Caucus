/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** NLRDUP.   Remove duplications of STR values in namelist P.
/
/    Removes entries from namelist P that have the same N'th word. */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CR  2/15/89 15:28 sysfree() must take a (char *). */
/*: JV  3/17/90 19:51 Removed stdio.h */
/*: DE  4/14/92 16:19 Chixified */
/*: CR  2/05/93 15:12 chxfree sp, sr. */
/*: CR  3/20/95 13:09 Bail out on <CANCEL>. */

#include "caucus.h"

extern union null_t null;

FUNCTION  nlrdup (p, n)
   struct namlist_t *p;
   int    n;
{
   struct namlist_t *q, *r;
   Chix   sp, sr;

   ENTRY ("nlrdup", "");

   if (p == null.nl)  RETURN(0);

   sp       = chxalloc (L(60), THIN, "nlrdup sp");
   sr       = chxalloc (L(60), THIN, "nlrdup sr");

   for (;   p->next != null.nl;   p=p->next) {
      chxtoken (sp, null.chx, n, (p->next)->str);
      for (q=p->next;    q!=null.nl  &&  q->next!=null.nl;    q=q->next) {
         if (sysbrktest())  break;
         r = q->next;
         chxtoken  (sr, null.chx, n, r->str);
         if (chxeq (sr, sp)) {
            q->next = r->next;
            chxfree (r->str);
            sysfree ((char *) r);
         }
      }
      if (sysbrktest())  break;
   }

   chxfree (sp);
   chxfree (sr);
   RETURN(1);
}
