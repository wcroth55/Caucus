/*** CREATEITEM.  Create a completely empty item, and increment the
/    number of items in the master file.  Returns the number of the
/    new item on success, 0 on failure.
/
/    CREATEITEM locks the master file, increments the # of items, and
/    creates a small partfile with the title "An Empty Item"
/       a completely empty response partfile
/
/    and then and *only* then rewrites the master file.  Thus, in case of
/    a crash during the item creation process, the master file and the
/    old items remain entirely consistent.
/
/    HOST is the name of the originating host for this item.  An empty
/    string means this host.  ITEM is the number of this item on the
/    originating host. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CR  4/28/89 13:00 Check return from unit_lock(XCMA... */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  6/22/91 16:09 Add host, item arguments. */
/*: CR  6/25/91 15:27 Add 'confnum' arg. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR 11/14/91 11:49 Pass master as arg to store_master() call. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: CR  2/24/92 15:31 Clear orighost for created local item. */
/*: DE  5/26/92 11:28 Change assignment to (master) to match new arg type */
/*: JX  5/25/91 14:50 Clean. */
/*: CI 10/08/92 10:46 Chix Integration. */
/*: CR 10/08/92 22:56 Use EMPTYCHX, not NULLSTR. */
/*: JV 11/17/92 17:41 Change orighost from Chix to short. */

#include <stdio.h>
#include "caucus.h"

extern struct master_template master;
extern Userreg                thisuser;
extern Chix *hostnames;
extern int   numhosts;

FUNCTION  createitem (host, item, confnum)
   Chix   host;
   int    item, confnum;
{
   int    success = 0, found, j;

   ENTRY ("createitem", "");

   /*** Lock the master file, increment the number of items. */
   if (NOT unit_lock  (XCMA, WRITE, L(confnum), L(0), L(0), L(0), nullchix))  FAIL;
   srvldmas   (0, &master, confnum);
   master.responses[++master.items] = 0;

   master.orighost[master.items] = 0;

   if (NOT EMPTYCHX(host)) {
      found = 0;
      /*** Is original hostname already in table? */
      for (j = 1; j <= numhosts; j++)
         if (chxeq (hostnames[j], host)) { found = 1; break; }
      /*** No, add it to the end. (Slot zero is not used!) */
      if (NOT found) {
         if (hostnames[++numhosts] == nullchix)
            hostnames[numhosts] = chxalloc (L(32), THIN, "cre_item hostname");
         chxcpy (hostnames[numhosts], host);
      }
      /*** Link orighost[i] to hostnames table. */
      if (found) master.orighost[master.items] = j;
      else       master.orighost[master.items] = numhosts;
   }

   master.origitem[master.items] = item;

   /*** Write a fake partfile. */
   unit_lock  (XCRF, WRITE, L(confnum), L(master.items), L(0), L(0), nullchix);
   unit_make  (XCRF);
   unit_write (XCRF, CQ("#0 "));
   unit_write (XCRF, thisuser->id);
   unit_write (XCRF, CQ(" 0 1/1/84 0000 SYSTEM\n An Empty Item\n"));
   unit_close (XCRF);
   unit_unlk  (XCRF);
  
   /*** Add this item to "Uncategorized Items" and the user's personal
   /    agenda category. */
/*
   unit_lock  (UAL, 0, L(L(0)), "");
   unit_make  (UAL);
   ageadditem (UNCATHEAD,      master.items);
   ageadditem (user.agendaptr, master.items);
   unit_close (UAL);
   unit_unlk  (UAL);
*/

   store_master (&master);
   unit_unlk    (XCMA);

   DONE (master.items);

 done:
   RETURN (success);
}
