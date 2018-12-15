
/*** STORE_MASTER.  Write master conference file from structure MASTER. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CR  2/16/89 10:47 Add check for 3B2 bug. */
/*: CR  3/27/89 13:58 Report corrupted master.items to buglist(), also. */
/*: CR  7/13/89 13:05 Check for corrupted master.organizerid also. */
/*: CR  6/19/91 13:31 Write origitem[] & orighost[] fields in master. */
/*: CX 10/20/91 16:49 Chixify. */
/*: CR 11/14/91 11:45 Take master struct as an argument. */
/*: CR  1/02/92 14:36 Add master.chgsubj. */
/*: JX  5/15/92 17:27 Terminate previous carvmark. */
/*: JX  9/28/92 16:04 Fix origitem bug. */
/*: CR 10/08/92 15:35 Test both origitem & orighost. */
/*: JV 10/14/92 10:27 Change sanity limit on master.items to MAXITEMS. */
/*: JV 11/17/92 17:35 Change orighost from Chix to short. */

#include <stdio.h>
#include "caucus.h"

extern union  null_t          null;
extern Chix  *hostnames;

FUNCTION  store_master (m)
   struct master_template *m;
{
   Chix   temp;
   char   buf[80];
   short  i;

   ENTRY ("storemast", "");

   /*** Check for 3B2 bug. */
   if (m->items < 0  ||  m->items > MAXITEMS  ||
                             EMPTYCHX(m->organizerid)) {
      buglist ("STORE_MASTER: master.items corrupted: fatal error.");
      printf  ("STORE_MASTER: master.items corrupted: fatal error.\n");
      exit    (1);
   }

   unit_make  (XCMA);
   unit_write (XCMA, m->organizerid);
   unit_write (XCMA, NEWLINE);

   temp = chxalloc (L(100), THIN, "storemast temp");

   sprintf (buf, "%d %d %d %d\n", m->items, m->nochange, m->noenter,
                                  m->chgsubj);
   unit_write (XCMA, CQ(buf));

   for (i=1;  i<=m->items;  ++i)  {
      chxclear  (temp);
      chxformat (temp, CQ("%d"), (int4) m->responses[i], L(0), 
                                        null.chx, null.chx);

      if (m->origitem[i] > 0  &&  m->orighost[i] > 0) 
         chxformat (temp, CQ(" %d %s"), (int4) m->origitem[i], L(0),
                                        hostnames[m->orighost[i]], null.chx);

      chxcat (temp, NEWLINE);
      unit_write (XCMA, temp);
   }
   unit_close (XCMA);

   chxfree (temp);
   RETURN  (1);
}
