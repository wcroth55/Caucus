/*** LOAD_HEADER.  Loads the header string for ITEM into STR, and
/    the title into TITLE.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  6/24/91 16:03 Skip "+" control lines. */
/*: CX 10/17/91 12:50 Chixify. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: CR  1/20/92 16:39 Add cnum argument. */
/*: DE  6/01/92 16:56 Patched unit_read */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR  1/21/93 18:08 Trim leading blank off returned title. */
/*: JV  4/02/93 10:44 Parse attachment header. */
/*: JJ  7/21/93 15:46 Reset thisitem.rnum. */

#include <stdio.h>
#include "caucus.h"

extern struct this_template thisitem;

FUNCTION  load_header (cnum, item, str, title, attach)
   int  item;
   Chix str, title;
   Attachment attach;
{
   Chix  temp;
   int4  v;

   ENTRY ("loadhead", "");

   if (NOT unit_lock (XCRF, READ, L(cnum), L(item), L(0), L(0), nullchix))
                                                         RETURN(0);
   if (NOT unit_view (XCRF))      { unit_unlk (XCRF);    RETURN(0); }

   temp = chxalloc (L(90), THIN, "load_head temp");

   unit_read  (XCRF, str, L(0));

   while (unit_read (XCRF, temp, L(0)) && chxvalue (temp, L(0)) == '+') {
      if (chxvalue (temp, L(1)) == '2')
         par_att_header (attach, temp);
   }

   chxclear  (title);
   chxcatsub (title, temp, L(1), ALLCHARS);

   unit_close (XCRF);
   unit_unlk  (XCRF);

   chxtoken (temp, nullchix, 3, str);
   chxnum   (temp, &v);
   thisitem.frozen = v;
   thisitem.number = item;
   thisitem.rnum   = 0;
   thisitem.cnum   = cnum;
   chxfree (temp);

   RETURN (1);
}
