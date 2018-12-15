
/*** ITEMDEL.   Delete a particular ITEM.
/
/    ok = itemdel (cnum, item);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    cnum;        (conference number)
/       int    item;        (item number on local host)
/
/    Purpose:
/       Delete an entire ITEM in conference CNUM.
/
/    How it works:
/       Delete the item and all of its responses and attachments.
/       Mark the item as deleted in masteres.
/
/    Returns:
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  chg_ditem()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  resp/itemdel.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CR  4/28/89 13:02 Check return from unit_lock (XCMA... */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  6/25/91 15:27 Add 'confnum' arg to srvldmas() calls. */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CR  8/07/91 14:25 Add 'confnum' arg to syspdir() call. */
/*: CX 10/17/91 13:25 Partially chixified. */
/*: CR 10/29/91 11:45 Add confnum arg to getline() call. */
/*: CR 11/14/91 11:49 Pass master as arg to store_master() call. */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: CR 11/19/91 11:32 Add CNUM argument. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: JV 12/16/91 16:42 Change pdir.lnum to lock. */
/*: JX  5/15/92 17:10 Remove comment-end marker in middle of header! */
/*: JV  5/03/93 13:14 Delete any attachments. */
/*: CP  5/06/92 12:30 Add L(0) as modnames 'item' arg. */
/*: CP  5/11/93  9:57 Use delnames() to delete date-entered files. */
/*: CP  7/14/93 14:16 Integrate 2.5 changes. */

#include <stdio.h>
#include "caucus.h"

extern struct master_template master;
extern struct pdir_t          pdir;
extern union  null_t          null;
extern short                  confnum;
extern Userreg                thisuser;

FUNCTION  itemdel (cnum, item)
   int    cnum, item;
{
   struct namlist_t *add, *del, *nlnames(), *nlnode();
   short  i;
   Chix   title, itemstr;
   Chix   s, getline2();

   ENTRY ("itemdel", "");

   /*** Get the title for this item, and remove the words in the title
   /    from the title name-files. */
   if ( (s = getline2 (XCRD, XCRF, cnum, nullchix, item, 0, 1, (Textbuf) NULL))
           == nullchix)
      RETURN (0);
   itemstr = chxalloc (L(10), THIN, "itemdel itemstr");
   title   = chxalloc (L(10), THIN, "itemdel title");
   chxcpy    (title, s);
   chxformat (itemstr, CQ("%03d"), (int4) item, L(0), nullchix, nullchix);
   del = nlnames (title, itemstr);
   add = nlnode  (1);
   modnames (XCTD, XCTN, nullchix, cnum, L(0), add, del, NOPUNCT);
   nlfree   (del);

   /*** Remove the words in the AUTHOR's name from the author name-files. */
   del = nlnames (thisuser->id, itemstr);
   modnames (XCAD, XCAN, nullchix, cnum, L(0), add, del, NOPUNCT);
   nlfree  (add);
   nlfree  (del);
   chxfree (itemstr);
   chxfree (title);

   /*** Delete the date-response-entered namelist file(s) for this item. */
   delnames (XCFD, XCFN, cnum, L(item));

   /*** Then mark item as deleted in master file. */
   unit_lock (XCRD, WRITE, L(cnum),  L(item), L(0), L(0), nullchix);
   if (unit_lock (XCMA, WRITE, L(cnum), L(0), L(0), L(0), nullchix)) {
      srvldmas  (0, &master, cnum);
      master.responses[item] = -1;
      store_master(&master);
      unit_unlk (XCMA);
  
      /*** Go through the the attachments directory and delete them. */
      syspdir (&pdir, XARF, cnum, item, nullchix);
      for (i=0;   i<pdir.parts;   ++i) {
         if (unit_lock (XARF, WRITE, L(cnum), L(item), L(pdir.resp[i]),
                                          L(0), nullchix)) {
            unit_kill (XARF);
            unit_unlk (XARF);
         }
      }

      /*** Finally, lock and load the item and response partfile
      /    directories for this item.  Go through the partfiles
      /    one by one and delete them. */
      syspdir (&pdir, XCRF, cnum, item, nullchix);
      for (i=0;   i<pdir.parts;   ++i) {
         unit_lock (XCRF, WRITE, L(cnum), L(item), L(pdir.resp[i]),
                                          L(pdir.lnum[i]), nullchix);
         unit_kill (XCRF);
         unit_unlk (XCRF);
      }
   }
   unit_unlk (XCRD);
  
   RETURN  (1);
}
