/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** NLFREE.   Free a NameList. */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: JV  8/30/88 11:49 Made more portable by casting p on sysfree call.*/
/*: JV  3/17/90 19:51 Removed stdio.h */
/*: CX 10/17/91 13:19 Chixify.  Put back <stdio.h>. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  nlfree (p)
   struct namlist_t *p;
{
   struct namlist_t *q;

   ENTRY ("nlfree", "");

   while (p != null.nl) {
      q = p->next;
      chxfree (p->str);
      sysfree ((char *) p);
      p = q;
   }

   RETURN (1);
}
