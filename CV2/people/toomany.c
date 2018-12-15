/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** TOOMANY.  Complain that too many people matched a certain name. 
/
/    Given a list of userids in OWNERS, complain that the string in
/    NAME matched the full names of each userid.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CR  3/01/89 15:47 Test for <CANCEL> during display. */
/*: JV  9/06/89 10:46 Added number param to person_is call. */
/*: CW  5/18/89 15:36 Add XW unit codes. */
/*: DE  4/29/92 14:15 Chixified */

#include "caucus.h"
#include <stdio.h>

extern union null_t null;

FUNCTION  toomany (name, owners)
   Chix   name;
   struct namlist_t *owners;
{
   struct namlist_t *p;
   Chix   id, format, newstr;
   short i=0;

   ENTRY ("toomany", "");

   id       = chxalloc (L(50),  THIN, "toomany id");
   format   = chxalloc (L(80),  THIN, "toomany format");
   newstr   = chxalloc (L(160), THIN, "toomany newstr");

   mdstr   (format, "ppl_Tanyone", null.md);
   chxformat (newstr, format, L(0), L(0), name, null.chx);
   unit_write (XWTX, newstr);

   sysbrkclear();
   for (p=owners->next;   p!=null.nl;   p = p->next) {
      if (sysbrktest())  break;
      chxtoken  (id, null.chx, 2,  p->str);
      person_is (XWTX, id, 0, null.par, ++i);
   }
   unit_write (XWTX, CQ("\n"));

   chxfree ( id );
   chxfree ( format );
   chxfree ( newstr );

   RETURN (1);
}
