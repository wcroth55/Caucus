/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** NLPRINT.   Print a NameList. */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CW  5/18/89 15:36 Add XW unit codes. */
/*: JV  3/17/90 19:51 Removed stdio.h */
/*: JX  5/21/92 11:20 Chixify. */

#include  <stdio.h>
#include "handicap.h"
#include "unitcode.h"
#include "chixuse.h"
#include "namelist.h"
#include "null.h"

extern union null_t null;

FUNCTION  nlprint (struct namlist_t *p,  char *str)
{
   Chix   newstr;

   ENTRY ("nlprint", "");

   newstr    = chxalloc (L(100), THIN, "nlprint newstr");

   chxformat  (newstr, CQ("-----NLPRINT: %s\n"), L(0), L(0), 
               chxquick (str, 1), null.chx);
   unit_write (XWTX, newstr); 
/* fprintf (stderr, "\n%s\n", ascquick(newstr)); */
/* fflush  (stderr); */
   for (;   p!=null.nl;   p=p->next) {
      chxclear   (newstr);
      chxformat  (newstr, CQ(" %4d '%s'\n"), L(p->num), L(0), p->str, null.chx);
      unit_write (XWTX, newstr);
   }

   chxfree (newstr);

   RETURN (1);
}
