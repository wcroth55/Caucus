
/*** ITEMFREEZE.  Freeze ITEM so no more responses may be added.
/
/    ITEMFREEZE modifies the "#" flag in the first item partfile
/    to mark this item as frozen.  VALUE means:
/       1  frozen by organizer or author of item.
/       2  frozen by Caucus; cannot be thawed.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CR  8/03/89 14:26 Lock XCRF for WRITE.  Add VALUE argument. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  6/26/91 14:30 Add 'confnum' argument to itemfreeze(). */
/*: CX 10/18/91 14:18 Chixified. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: CR 12/26/91 12:00 Last arg to loadtext() must be int4. */
/*: CR 12/07/92 13:40 Add CONFNUM arg to loadtext(). */
/*: CR 12/10/92 15:15 chxalter arg int4. */
 
#include <stdio.h>
#include "caucus.h"

extern struct this_template  thisitem;
extern Textbuf               tbuf;

FUNCTION  itemfreeze (confnum, item, value)
   short  item;
   int    value, confnum;
{
   Chix   oldvalue, newvalue;
   int4   pos;

   ENTRY ("itemfreeze", "");

   unit_lock (XCRF, WRITE, L(confnum), L(item), L(0), L(0), nullchix);
   loadtext  (XCRF, confnum, item, 0, L(0), tbuf);

   oldvalue = chxalloc (L(20), THIN, "itemfree oldvalue");
   newvalue = chxalloc (L(20), THIN, "itemfree newvalue");

   pos = chxtoken (oldvalue, nullchix, 3, tbuf->line[0]);
   chxformat (newvalue, CQ("%d"), (int4) value, L(0), nullchix, nullchix);
   chxalter  (tbuf->line[0], pos, oldvalue, newvalue);

   chxfree (oldvalue);
   chxfree (newvalue);

   storetext (XCRF, tbuf->r_first, tbuf->r_last, tbuf);
   unit_unlk (XCRF);

   thisitem.frozen = value;
   RETURN (1);
}
