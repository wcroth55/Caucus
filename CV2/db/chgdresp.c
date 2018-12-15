/*** CHG_DRESP.  Add or change a response in the Caucus database.
/
/    ok = chg_dresp (action, cnum, localinum, origrnum, localrnum, respchn,
/              author, id, text, date, time, attach, prop, copied, error);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    action;      (XT_CHANGE/XT_NEW/XT_DELETE)
/       int    cnum;        (conference number)
/       int    localinum;   (item number on local host)
/       int    origrnum;    (response number on original host)
/       int   *localrnum;   (response number on local host)
/       Chix   respchn;     (name of original host)
/       Chix   author;      (name of author)
/       Chix   userid;      ("id@host" of author)
/       Chix   text;        (text of resp)
/       Chix   date;        (date response was created)
/       Chix   time;        (time response was created)
/       Attachment attach;  (info about response attachment)
/       int    prop;        (text property code)
/       Chix   copied;      (info about response was copied from)
/       int   *error;       (return error code)
/
/    Purpose:
/       Chg_dresp() is the one true way of adding a response to a conference,
/       changing the text of an existing response, or deleting a response.
/       All Caucus and CaucusNet code that adds or changes responses must
/       call this function.
/
/    How it works (when adding a new response):
/       Cnum, localinum, author, text, date, time are required.  The
/       new response number will be returned in localrnum.
/
/       If the response is first created on this host, respchn is "".
/       Userid may be a simple id (without "@host").
/
/       If the item is being imported from another host, respchn must
/       be the name of that host.  Origrnum must be the number of the new
/       response on that host.  Userid is id of original author of response.
/       (If it does not contain "@host", chg_dresp() will use respchn.)
/
/       Use attachment arg in updatepart() call.
/
/    How it works (when changing an existing response):
/       Localrnum must be the local response number.  Cnum and
/       localinum are required.  Text contains the replacement text of
/       the response.  If author or userid are not null, they replace
/       the previous values.  Origrnum, respchn, date, and time
/       are ignored.
/
/       Use attachment arg in updatepart() call.
/
/    How it works (when deleting a response):
/       Localrnum must be the local response number.  Cnum and
/       localinum are required.  Text, origrnum, respchng, author,
/       userid, date, and time are ignored.
/
/       The attach arg is ignored.
/
/    Returns: 1 on success, sets ERROR to 0.
/             1 if item used to exist, but was deleted; sets ERROR to 0.
/             0 on any error condition, see below.
/
/    Error Conditions: sets ERROR accordingly:
/       DBADACT      bad action code
/       DINOITEM     no such item localinum
/       DNOCONF      cannot lock or read XCMA masteres file
/       DINORESP     no such local response
/       DBADDB       cannot lock resp partfile or add response
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
/    Home:  db/chgdresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/25/91 16:56 New function. */
/*: CR  7/05/91 12:51 Use DNOCONF error code. */
/*: CR  7/26/91 18:30 Use null.str instead of NULL. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: CR 10/29/91 11:50 Add cnum argument. */
/*: CR 11/14/91 11:49 Pass master as arg to store_master() call. */
/*: CR 11/15/91 12:45 Error if XT_NEW and response already exists. */
/*: JV 11/15/91 15:14 Validate input. */
/*: CR 11/19/91 12:06 Return 1 if item used to exist but was deleted. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: DE  5/26/92 11:58 Fixed getline2 args & a chxcpy */
/*: JX  5/26/92 14:01 Small chix fix. */
/*: CX  5/28/92 18:02 Add tbuf arg to updatepart() call. */
/*: JX  5/29/92 14:34 Chixify arg8 to updatepart() call. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: JV  2/26/93 23:15 Add attachment arg. */
/*: CP  5/10/93 23:45 Call moddate() to add/del datemap entries. */
/*: CP  7/14/93 14:56 Integrate 2.5 changes. */
/*: CP  8/30/93 13:11 Clear related API textbuf. */
/*: CK  9/16/93 22:09 Declare a_cch_tbuf(). */
/*: CK 10/13/93 19:02 Check new resp against license.respmax. */
/*: CR  6/23/94 15:56 Clear api cache that would hold new response. */
/*: CR  6/16/95 16:12 Add text properties. */
/*: CR  3/17/00 15:01 Author and Userid values can be changed by XT_CHANGE. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"
#include "derr.h"
#include "api.h"

extern struct master_template  master;
extern union  null_t           null;
extern Textbuf                 tbuf;
extern short                   confnum;

FUNCTION  chg_dresp (action, cnum, localinum, origrnum, localrnum, respchn,
                author, id, text, date, time, attach, prop, copied, error)
   int    cnum, localinum, origrnum, *localrnum, prop, *error;
   Chix   respchn, author, id, date, time;
   Chix   text, copied;
   Attachment attach;
{
   int     oresp, ok, success, wc;
   Chix    empty, newhead, userid, host, oldhead, word;
   Chix    getline2();
   Textbuf a_cch_tbuf();

   ENTRY ("chg_dresp", "");

   newhead   = chxalloc (L(160), THIN, "chgdresp newhead");
   oldhead   = chxalloc (L(160), THIN, "chgdresp oldhead");
   userid    = chxalloc (L(80),  THIN, "chgdresp userid");
   host      = chxalloc (L(80),  THIN, "chgdresp host");
   word      = chxalloc (L(80),  THIN, "chgdresp word");
   empty     = chxalloc (L(10),  THIN, "chgdresp empty");
   success   = 0;

   /*** Validate input. */
   if (cnum < 1 || localinum < 0) {
      *error = DINOITEM; FAIL; }

   /*** Check action code. */
   if (action != XT_NEW  &&  action != XT_CHANGE  &&  action != XT_DELETE)
      { *error = DBADACT;    FAIL; }

   /*** Load XCMA/master if needed.  (XT_NEW keeps it locked as we're about
   /    to increment the # of responses on this item.) */
   confnum = cnum;
   if (action == XT_NEW) {
      if (NOT unit_lock (XCMA, WRITE, L(cnum), L(0), L(0), L(0), null.chx))
         { *error = DNOCONF;                       FAIL; }
      if (NOT srvldmas  (0, &master, cnum))
         { *error = DNOCONF;   unit_unlk (XCMA);   FAIL; }
   }
   else {
      /*** XT_CHANGE and XT_DELETE only need to load XCMA if wrong
      /    conference number. */
      if (master.cnum != cnum  &&  NOT srvldmas (1, &master, cnum))
         { *error = DNOCONF;                       FAIL; }
   }

   /*** Make sure the item (containing the response) actually exists. */
   if (localinum > master.items) {
      if (action == XT_NEW)  unit_unlk (XCMA);
      *error = DINOITEM;
      FAIL;
   }
   if (DELETED (localinum)) {
      if (action == XT_NEW)  unit_unlk (XCMA);
      *error = 0;
      SUCCEED;
   }


   /*** Changing an existing response? */
   if (action == XT_CHANGE) {

      /*** Make sure the response to be changed really exists. */
      if (getline2 (XCRD, XCRF, cnum, null.chx, localinum, *localrnum, 0,
          (Textbuf) NULL) == nullchix)
         { *error = DINORESP;   FAIL; }

      /*** Get the orig resp and host info for this response. */
      ok = getoresp (&oresp, host, cnum, localinum, *localrnum);
      if (NOT ok)  { *error = DINORESP;   FAIL; }

      /*** Replace the response text.   Get the old header and replace
      /    owner and author if set.  Then replace the new text by updating
      /    the partfile containing it and the header. */
      chxcpy (oldhead, getline2(XCRD, XCRF, cnum, null.chx, localinum,
                *localrnum, 0, (Textbuf) NULL));
      chxclear (newhead);
      for (wc=1;   wc<6;   ++wc) {  
         chxtoken  (word, nullchix, wc, oldhead);
         if (wc == 2  &&  id != nullchix)  chxcpy (word, id);
         chxcat    (newhead, word);
         chxcatval (newhead, THIN, L(' '));
      }
      chxtoken (nullchix, word, 6, oldhead);
      if (author != nullchix)  chxcpy (word, author);
      chxcat (newhead, word);

      if (ok = unit_lock (XCRD, WRITE, L(cnum), L(localinum), L(0), L(0),
              null.chx)) {
         ok = updatepart (XCRF, cnum, localinum, *localrnum, null.chx, text,
                   newhead, nullchix, oresp, host, attach, tbuf, prop, copied);
         unit_unlk (XCRD);
      }
      if (NOT ok)  { *error = DBADDB;   FAIL; }
   }

   /*** Adding a new response? */
   else if (action == XT_NEW) {

      /*** If importing an item, make sure userid is "id@host". */
      chxcpy (userid, id);
      if (NOT EMPTYCHX(respchn)  &&  chxindex (userid, L(0), CQ("@")) < 0) {
         chxcat (userid, CQ("@"));
         chxcat (userid, respchn);
      }

      /*** Get the (new) response number.  Clear the api cache that
      /    would presumably hold this new response. */
      *localrnum = ++master.responses[localinum];
      a_cch_tbuf (V_CLR, XCRD, XCRF, cnum, localinum, *localrnum-1, nullchix);
      store_master (&master);

      /*** Freeze this item if it is full. */
      if (*localrnum >= MAXRESP - 5)            itemfreeze (cnum, localinum, 2);
      unit_unlk (XCMA);
  
      /*** Add the response text. */
      chxclear  (newhead);
      chxformat (newhead, CQ("#%04d %s 0 %s"), L(*localrnum), L(0), userid,
                 date);
      chxformat (newhead, CQ(" %s %s"), L(0), L(0), time, author);
      if (ok = unit_lock (XCRD, WRITE, L(cnum), L(localinum), L(0), L(0),
                             null.chx)) {
         ok = updatepart (XCRF, cnum, localinum, *localrnum, null.chx, text,
                          newhead, nullchix, origrnum, respchn, attach, 
                          tbuf, prop, copied);
         unit_unlk (XCRD);
      }
      if (NOT ok)  { *error = DBADDB;   FAIL; }

      /*** Add a "RTiii001" response-time-entry. */
      moddate (1, XCFD, XCFN, cnum, L(localinum), date, time, L(*localrnum));
   }

   /*** Delete response. */
   else if (action == XT_DELETE) {
      chxclear  (newhead);
      chxformat (newhead, CQ("#%d"), L(*localrnum), L(0), null.chx, null.chx);
      if (ok = unit_lock (XCRD, WRITE, L(cnum), L(localinum), L(0), L(0),
                                null.chx)) {
         ok = updatepart (XCRF, cnum, localinum, *localrnum, null.chx, empty,
                                newhead, nullchix, -1, nullchix, attach, 
                                tbuf, prop, copied);
         unit_unlk (XCRD);
      }
      if (NOT ok)  { *error = DBADDB;   FAIL; }
   }

   /*** Clear any api textbuf that might have had a copy of this response
   /    before it was changed... */
   a_cch_tbuf (V_CLR, XCRD, XCRF, cnum, localinum, *localrnum, nullchix);

   *error = NOERR;
   SUCCEED;

done:
   chxfree (newhead);
   chxfree (oldhead);
   chxfree (userid);
   chxfree (host);
   chxfree (word);
   chxfree (empty);
  
   RETURN  (success);
}
