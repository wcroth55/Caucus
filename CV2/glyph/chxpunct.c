/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CHXPUNCT.   Copy OLD to NEW, removing punctuation characters. */

/*: JX  5/27/92 12:04 New function. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR  2/05/93 15:07 chxfree punct. */
/*: CR 10/12/05 Remove mdstr() for C5. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"

FUNCTION  chxpunct (new, old)
   Chix   new, old;
{
   static Chix s_punct = nullchix;
   Chix   punct;
   int4   oldchar;
   int    plen, clen, found, i, p;

   ENTRY ("chxpunct", "");

   punct = chxalloc (L(20), THIN, "chxpunct punct");

   chxcpy (punct, CQ("`~@$%^&*()-=+\\|[]{};:\"',.<>/?"));

   chxclear (new);
   clen = chxlen (old);
   plen = chxlen (punct);
   for (i=0;   i<clen;   i++) {
      oldchar = chxvalue (old, L(i));
      for (p=0; p<plen; p++) {
         found = (oldchar == chxvalue (punct, L(p)));
         if (found)  break;
      }
      if  (found) continue;

      chxcatval (new, jixwidth (oldchar), oldchar);
   }

   chxfree (punct);
   RETURN (1);
}
