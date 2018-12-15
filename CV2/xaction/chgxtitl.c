/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CHGXTITLE. Create a xaction to change the title of an item.
/
/   int n = chgxtitle (confnum, itemnum, itemchn, new_title, error)
/
/   Parameters:
/      int   confnum, itemnum, *error;
/      char *itemchn, *new_title;
/
/   Purpose:
/      Caucus_x should call this to generate a xaction so that the title
/      of this item gets changed on all the other appropriate hosts.
/      It does not affect the local Caucus DB in any way.
/
/   How it works:
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/
/   Error Conditions:
/
/   Related Functions:
/
/   Called by:
/
/   Home: xaction/chgxtitl.c
/ */

/*: JV  6/05/91 12:20 Create this. */
/*: JV  6/10/91 18:15 Change arg list, add code. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 19:14 Clean. */
/*: JV  7/09/91 10:40 Completely rewrite to use putnextxact(). */
/*: JV  7/15/91 20:53 Add multiple text fields. */
/*: CR  7/27/91 14:17 Include "goodlint.h". */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: JV  7/29/91 12:26 Add arg to get_trueconf */
/*: JV 11/06/91 10:29 Remove common.h, add chixuse.h. */
/*: JV 11/12/91 12:03 Add confochn. */
/*: JV 11/19/91 10:00 Actually get mychn before using it... */
/*: JV 11/26/91 13:32 Send chg's generated at OCHN to XCXT, not XHML. */
/*: JV 12/04/91 14:42 Add SUBNUM field. */
/*: JV 12/07/91 19:28 Declare chxalloc(). */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: JV  1/17/92 16:24 Remove old deadcode. */
/*: DE  6/03/92 11:03 Chixified */
/*: JX  9/01/92 12:54 Change empty_xact to make_xaction(). */

#include "chixuse.h"
#include "done.h"
#include "null.h"
#include "handicap.h"
#include "derr.h"
#include "unitcode.h"
#include "xaction.h"
#include "goodlint.h"

extern union null_t null;

FUNCTION  chgxtitle (confnum, itemnum, itemchn, new_title, error)
   int   confnum, itemnum, *error;
   Chix itemchn, new_title;
{
   Chix tname, locname, orighost, mychn, subscriber;
   int  lhnum, success;
   Xactionptr xact, make_xaction();
   Chix carray[2];

   ENTRY ("chgxtitle", "");

   tname      = chxalloc (L(80), THIN, "chgxtitl tname");
   locname    = chxalloc (L(80), THIN, "chgxtitl locname");
   orighost   = chxalloc (L(80), THIN, "chgxtitl orighost");
   mychn      = chxalloc (L(80), THIN, "chgxtitl mychn");
   subscriber = chxalloc (L(80), THIN, "chgxtitl subscriber");
   success    = 0;

   xact = make_xaction();

   if (NOT get_chn (mychn, XSNF, 0)) {
      bug ("CHGXTITLE: can't get XSNF.", 0); FAIL; }

   get_trueconf (confnum, locname, tname, orighost);

   xact->typenum = TT_ITEM;
   xact->subnum  = SUBTITLE;
   xact->actnum  = XT_CHANGE;
   if (NOT chxeq (orighost, mychn))
      xact->transit = 1;
   chxcpy (xact->conftname, tname);
   chxcpy (xact->confochn,  orighost);
   xact->conflocnum = confnum;
   xact->itemid = itemnum;
   chxcpy (xact->itemchn, itemchn);
   chxcpy (xact->title, new_title);
/* if (date[0] != '\0')
      sprintf (xact->date, "%s %s", date, time); */

   if (chxeq (mychn, orighost)) {
      if (NOT unit_lock (XCXT, WRITE, L(confnum), L(0), L(0), L(0), null.chx)) {
         bug ("CHGXTITLE: can't lock XCXT.", 0); *error=DLOCK; FAIL; }
      if (NOT unit_append (XCXT)) {
         bug ("CHGXTITLE: can't open XCXT.", 0);
         unit_unlk (XCXT);
         *error=DFILE;  FAIL; }
      carray[0] = carray[1] = chxalloc (L(0), THIN, "chgxtitle carray");
      putnextxact (XCXT, xact, carray);
      unit_close (XCXT); unit_unlk (XCXT);
      chxfree (carray[1]);                /* Frees both */

   } else {
      if (NOT get_course (orighost, &lhnum)) {
         bug ("CHGXTITLE: host not found.", 0);  *error=DNOCHN; FAIL; }
      if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
        bug ("CHGXTITLE: can't lock XHML.", 0);  *error=DLOCK;  FAIL; }
      if (NOT unit_append (XHML)) {
         bug ("CHGXTITLLE: can't open XHML.", 0); *error=DFILE;
         unit_unlk (XHML); FAIL; }
      carray[0] = carray[1] = chxalloc (L(0), THIN, "chgxtitle carray");
      putnextxact (XHML, xact, carray);
      chxfree (carray[0]);
      unit_close (XHML); unit_unlk (XHML);
   }
   SUCCEED;
 done:
   chxfree (tname);
   chxfree (locname);
   chxfree (orighost);
   chxfree (mychn);
   chxfree (subscriber);
   free_xaction (xact);

   RETURN (success);   
}   
