/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** NLSIZE.   Return the number of nodes (not counting head) in a namelist. */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: JV  3/17/90 19:51 Removed stdio.h */

#include "caucus.h"

extern union null_t null;

FUNCTION  nlsize (p)
   struct namlist_t *p;
{
   short  size;

   ENTRY ("nlsize", "");

   if  (p == null.nl)     RETURN (0);
   size = 0;
   for (p = p->next;   p!=null.nl;   p = p->next)  ++size;

   RETURN (size);
}
