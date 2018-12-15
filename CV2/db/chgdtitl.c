/*** Chg_DTITLE.  Change the title of an item in the Caucus database.
/
/    ok = chg_dtitle (cnum, localinum, title, error);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    cnum;        (conference number)
/       int    localinum;   (item number on local host)
/       Chix  *title;       (item title)
/       int   *error;       (return error code)
/
/    Purpose:
/       Chg_ditem() is the one true way of changing the title of an item
/       in a conference.  All Caucus and CaucusNet code that changes a
/       title must call this function.
/
/    How it works:
/
/    Returns: 1 on success, sets ERROR to 0.
/             1 if item used to exist, but was deleted; sets ERROR to 0.
/             0 on any error condition, see below.
/
/    Error Conditions: sets value of ERROR accordingly:
/       DNOCONF      cannot lock or read XCMA masteres file
/       DINOITEM     item does not exist
/  
/    Side effects: may reload global "master".
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/chgdtitl.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/01/91 16:08 New function. */
/*: CR  7/05/91 12:51 Use DNOCONF error code. */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: JV 11/19/91 10:25 Fix header. */
/*: CR 11/19/91 14:25 Return 1 if item was deleted. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JV 12/16/91 18:47 Change 4th arg to loadtext to int4. */
/*: CX  4/10/92 15:49 Replace block/line/etc with local Textbuf. */
/*: JX  5/26/92 14:37 Fix header. */
/*: CR 12/07/92 13:37 Add CNUM arg to loadtext(). */
/*: CP  5/06/92 12:30 Add L(0) as modnames 'item' arg. */
/*: CP  8/01/93 14:32 Add 'name' arg to make_textbuf() call. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"
#include "derr.h"

extern struct master_template  master;
extern union  null_t           null;
extern short                   confnum;

FUNCTION  chg_dtitle (cnum, localinum, title, error)
   int    cnum, localinum, *error;
   Chix   title;
{
   struct  namlist_t *add, *del, *nlnames(), *nlnode();
   Chix    str, oldtitle;
   int     ok, i, success;
   Textbuf tbuf, make_textbuf();

   ENTRY ("chg_dtitle", "");

   str      = chxalloc (L(40),  THIN, "chgdtitl str");
   oldtitle = chxalloc (L(160), THIN, "chgdtitl oldtitle");
   success  = 0;

   /*** Make sure this item actually exists. */
   confnum = cnum;
   if (master.cnum != cnum  &&  NOT srvldmas (1, &master, cnum))
                                  { *error = DNOCONF;    FAIL; }
   if (localinum > master.items)  { *error = DINOITEM;   FAIL; }
   if (DELETED (localinum))                              SUCCEED;

   /*** Lock and reload the first partfile for this item.  Hook in the
   /    new title, and rewrite the whole partfile. */
   if (ok = unit_lock (XCRF, WRITE, L(cnum), L(localinum), L(0), L(0), nullchix)) {
      tbuf = make_textbuf (TB_RESPS, TB_LINES, "chgdtitl tbuf");
      if (ok = loadtext  (XCRF, cnum, localinum, 0, L(0), tbuf)) {
         i = tbuf->r_start[0] + 1;  /* Item title is 2nd line of resp 0. */
         chxcpy (oldtitle, tbuf->line[i]);
         chxcpy (tbuf->line[i], title);
         storetext (XCRF, tbuf->r_first, tbuf->r_last, tbuf);
      }
      free_textbuf (tbuf);
      unit_unlk (XCRF);
   }
   if (NOT ok)  { *error = DINOITEM;   FAIL; }

   /*** Replace the item's old title with the new title.  Create a list of
   /    nameparts to DELete, and a list of nameparts to ADD.  Hand them
   /    off to MODNAMES to do the dirty work. */
   chxformat (str, CQ("%03d"), L(localinum), L(0), nullchix, nullchix);
   add = nlnames (title,    str);
   del = nlnames (oldtitle, str);
   modnames (XCTD, XCTN, nullchix, cnum, L(0), add, del, NOPUNCT);
   nlfree (add);
   nlfree (del);

   *error = NOERR;

   SUCCEED;

done:

   chxfree (str);
   chxfree (oldtitle);

   RETURN (success);
}
