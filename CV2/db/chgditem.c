/*** CHG_DITEM.  Change, add, or delete an item in the Caucus database.
/
/    ok = chg_ditem (action, cnum, originum, localinum, itemchn, author,
/               userid, text, title, date, time, attach, prop, copied, error);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    action;      (XT_NEW, XT_CHANGE, XT_DELETE)
/       int    cnum;        (conference number)
/       int    originum;    (item number on original host)
/       int   *localinum;   (item number on local host)
/       Chix   itemchn;     (name of original host)
/       Chix   author;      (name of author)
/       Chix   userid;      ("id@host" of author)
/       Chix   text;        (text of item)
/       Chix   title;       (item title)
/       Chix   date;        (date item was created)
/       Chix   time;        (time item was created)
/       Attachment attach;  (info about item attachment)
/       int    prop;        (text property)
/       Chix   copied;      (info about item copied from where?)
/       int   *error;       (return error code)
/
/    Purpose:
/       Chg_ditem() is the one true way of adding an item to a conference,
/       changing the text of an existing item, or deleting an item.  All
/       Caucus and CaucusNet code that adds, changes, or deletes items must
/       call this function.
/
/    How it works (when adding a new item):
/       Cnum, author, text, title, date, time are required.  The
/       new item number will be returned in localinum.
/
/       If the item is first created on this host, itemchn is "".  Userid may
/       be a simple id (without "@host").
/
/       If the item is being imported from another host, itemchn must
/       be the name of that host.  Originum must be the number of the new
/       item on that host.  Userid is id of original author of item.
/       (If it does not contain "@host", chg_ditem() will use itemchn.)
/
/       Use attachment arg in updatepart() call.
/
/    How it works (when changing an existing item):
/       Localinum must be the local item number.  Cnum is required.
/       Text contains the replacement text of the item.  Author, userid,
/       title, date, time, originum, itemchn are ignored.
/
/       Use attachment arg in updatepart() call.
/ 
/    How it works (when deleting an item):
/       Localinum must be the local item number.  Cnum is required.  Text,
/       author, userid, title, date, time, originum, itemchn are ignored.
/
/       Use attachment arg in updatepart() call.
/
/    Returns: 1 on success, sets ERROR to 0.
/             1 if XT_NEW and item already exists, does nothing.
/             1 if local item used to exist but was deleted, sets ERROR to 0.
/             0 on any error condition, see below.
/
/    Error Conditions: sets value of ERROR accordingly:
/       DBADACT      bad ACTION code
/       DINOITEM     specified item does not exist
/       DNOCONF      cannot lock or read XCMA masteres file
/       DBADDB       cannot lock item partfile, or item creation failure
/       DCFULL       conference is already full, cannot create new item
/ 
/    Side effects: may reload globals master, confnum.
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/chgditem.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/19/91 14:26 New function. */
/*: CR  7/05/91 12:51 Use DNOCONF error code. */
/*: CR  7/17/91 11:47 Change comments. */
/*: CR  7/30/91 22:23 Chg_ditem() must not call subuncat()! */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: CR 11/15/91 12:21 Return 0 for XT_NEW if item already exists! */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: CR 11/19/91 11:34 Add CNUM to itemdel(), return 1 if deleted item. */
/*: CR 11/25/91 23:21 Ignore duplicate item. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: CR  1/14/92 12:47 Check localinum back from iorig_to_local(). */
/*: DE  3/25/92 17:48 Chixified */
/*: DE  5/26/92 12:10 Fixed calls to getline2 */
/*: CX  5/28/92 18:02 Add tbuf arg to updatepart() call. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: JV  4/05/93 17:03 Add attachment arg. */
/*: CP  5/06/92 12:16 Create XCFD/XCFN files for new item. */
/*: CP  5/06/92 12:30 Add L(0) as modnames 'item' arg. */
/*: CP  5/11/93 10:35 Fetch date/time to use when deleting item. */
/*: CN  5/16/93 12:31 Don't add XCFN entry for response 0. */
/*: CN  5/16/93 13:27 Add 'cnum' arg to initnmdir(). */
/*: CP  7/14/93 14:49 Integrate 2.5 changes. */
/*: CP  8/04/93 16:16 XT_NEW: add moddate() call to initializing "RTiii001". */
/*: CK  9/14/93 22:16 Clear any api tbuf that might have a copy of this item. */
/*: CK  9/16/93 22:09 Declare a_cch_tbuf(). */
/*: CK 10/13/93 19:01 Check # items against license.itemmax. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"
#include "derr.h"
#include "api.h"

extern struct master_template  master;
extern Textbuf                 tbuf;
extern short                   confnum;
extern union null_t            null;

FUNCTION  chg_ditem (action, cnum, originum, localinum, itemchn, author,
                     id, text, title, date, time, attach, prop, copied, error)
   int    action, cnum, originum, *localinum, prop, *error;
   Chix   itemchn, author, id, title, date, time;
   Chix   text, copied;
   Attachment attach;
{
   struct  namlist_t *add, *del, *nlnames(), *nlnode();
   Chix    newhead, number, userid, oldtitle;
   Chix    temphead;
   int     ok, success;
   Chix    getline2();
   int4    systime();
   Textbuf a_cch_tbuf();

   ENTRY ("chg_ditem", "");

   newhead    = chxalloc (L(200), THIN, "chgditem newhead");
   number     = chxalloc (L(20),  THIN, "chgditem number");
   userid     = chxalloc (L(80),  THIN, "chgditem userid");
   oldtitle   = chxalloc (L(200), THIN, "chgditem oldtitle");
   success    = 0;

   /*** Check action code. */
   *error = 0;
   if (action != XT_NEW  &&  action != XT_CHANGE  &&  action != XT_DELETE)
      { *error = DBADACT;    FAIL; }

   /*** For change and delete, make sure we have the right XCMA loaded,
   /    and that the item actually exists. */
   confnum = cnum;
   if (action == XT_CHANGE  ||  action == XT_DELETE) {
      if (master.cnum != cnum  &&  NOT srvldmas (1, &master, cnum))
                                      { *error = DNOCONF;    FAIL; }
      if (*localinum > master.items)  { *error = DINOITEM;   FAIL; }
      if (DELETED (*localinum))                              SUCCEED;
   }


   /*** Changing an existing item? */
   if (action == XT_CHANGE) {

      /*** Ensure that we can really get at the item with getline2(). */
      if (getline2 (XCRD, XCRF, cnum, null.chx, *localinum, 0, 1, (Textbuf) NULL)
                == nullchix)   { *error = DINOITEM;   FAIL; }

      /*** Get the old header and title. */
      chxcpy (newhead, getline2 (XCRD, XCRF, cnum, null.chx, *localinum, 0, 0,
                                 (Textbuf) NULL));
      chxcpy (oldtitle, getline2 (XCRD, XCRF, cnum, null.chx, *localinum, 0, 1,
                                 (Textbuf) NULL));

      /*** Replace the item text. */
      if (ok = unit_lock (XCRD, WRITE, L(cnum), L(*localinum), L(0), L(0), null.chx)) {
         ok = updatepart (XCRF, cnum, *localinum, 0, null.chx, text,
                  newhead, oldtitle, -1, nullchix, attach, tbuf, prop, copied);
         unit_unlk (XCRD);
      }
      if (NOT ok)  { *error = DBADDB;   FAIL; }
   }

   /*** Adding a new item. */
   else if (action == XT_NEW) {

      /*** If this is an imported item that already exists here,
      /    don't do anything, just return success. */
      if (NOT EMPTYCHX(itemchn)) {
         iorig_to_local (cnum, originum, itemchn, localinum);
         if (*localinum > 0) SUCCEED;
      }

      /*** Are there enough empty item slots left in this conference? */
      if (master.items >= MAXITEMS - 1)           { *error = DCFULL;   FAIL; }

      /*** If importing an item, make sure userid is "id@host". */
      chxcpy (userid, id);
      if (NOT EMPTYCHX(itemchn)  &&  chxindex (userid, L(0), CQ("@")) < 0) {
         chxcat (userid, CQ("@"));
         chxcat (userid, itemchn);
      }

      /*** Create a completely empty item. */
      if ( NOT (*localinum = createitem (itemchn, originum, cnum)))
         { *error = DBADDB;   FAIL; }

      /*** Replace the empty item with the item text and proper header. */
      if (ok = unit_lock (XCRD, WRITE, L(cnum), L(*localinum),
                              L(0), L(0), null.chx)) {
         chxclear  (newhead);
         chxformat (newhead, CQ("#0 %s 0 %s"), L(0), L(0), userid, date);
         chxformat (newhead, CQ(" %s %s"), L(0), L(0), time, author);
         ok = updatepart (XCRF, cnum, *localinum, 0, null.chx, text,
                    newhead, title, -1, null.chx, attach, tbuf, prop, copied);
         unit_unlk (XCRD);
      }
      if (NOT ok)  { *error = DBADDB;   FAIL; }

      /*** Add the words in the TITLE of this item to the title-namefiles. */
      chxformat  (number, CQ("%03d"), L(*localinum), L(0), null.chx, null.chx);
      add = nlnames (title, number);
      del = nlnode  (1);
      modnames (XCTD, XCTN, null.chx, cnum, L(0), add, del, NOPUNCT);
      nlfree   (add);

      /*** Add an entry for this item to the AUTHR001 files. */
      add = nlnames (userid, number);
      modnames (XCAD, XCAN, null.chx, cnum, L(0), add, del, NOPUNCT);
      nlfree (add);
      nlfree (del);

      /*** Add an entry for this item to the "ITIME001" item-time-entry
      /    files. */
      moddate (1, XCED, XCEN, cnum, L(0),            date, time, L(*localinum));

      /*** Initialize the "RTiii001" response-time-entry file. */
      initnmdir  (XCFD, XCFN, cnum, nullchix, L(*localinum));
      moddate (1, XCFD, XCFN, cnum, L(*localinum), date, time, L(0));
   }

   else if (action == XT_DELETE) {
      /*** Ensure that we can really get at the item with getline2(). */
      temphead = getline2 (XCRD, XCRF, cnum, null.chx, *localinum, 0, 0,
                                (Textbuf) NULL);
      if (temphead == nullchix) { *error = DINOITEM;   FAIL; }

      date = chxalloc (L(20), THIN, "chgditem date");
      time = chxalloc (L(20), THIN, "chgditem time");
      chxtoken (date, nullchix, 4, temphead);
      chxtoken (time, nullchix, 5, temphead);
      moddate  (0, XCED, XCEN, cnum, L(0), date, time, L(*localinum));
      chxfree  (date);
      chxfree  (time);

      if (NOT itemdel (cnum, *localinum)) { *error = DBADDB;   FAIL; }
   }

   /*** Clear any api textbuf that might have had a copy of this response
   /    before it was changed... */
   a_cch_tbuf (V_CLR, XCRD, XCRF, cnum, *localinum, 0, nullchix);

   *error = NOERR;
   SUCCEED;

 done:
   chxfree (newhead);
   chxfree (number);
   chxfree (userid);
   chxfree (oldtitle);

   RETURN  (success);
}
