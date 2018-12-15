/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** NLNODE.   Create a namelist node with strlength S. */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CR  2/15/89 15:28 sysfree() must take a (char *). */
/*: JV  3/17/90 19:51 Removed stdio.h */
/*: CX 10/17/91 13:18 Chixify.  Put back <stdio.h>. */
/*: CL 12/11/92 15:52 Long sysmem arg. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

char nlerror = 0;

FUNCTION  struct namlist_t *nlnode (s)
   int    s;
{
   struct namlist_t *p;
   char  *sysmem();

   ENTRY ("nlnode", "");

   p = (struct namlist_t *) sysmem (L(sizeof (struct namlist_t)), "namlist_t");
   if (p == null.nl) {
      nlerror = 1;
      RETURN (null.nl);
   }

   p->str  = chxalloc ( (int4) s, THIN, "nlnode str");
   p->next = null.nl;

   RETURN (p);
}
