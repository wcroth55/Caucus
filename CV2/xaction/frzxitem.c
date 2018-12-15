/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** FRZXITEM.  Create a FREEZE ITEM xaction.
/
/   int n = frzxitem (confnum, itemnum, itemchn, freeze, date, time, 
/                     dest_chn, error)
/
/   Parameters:
/      int   confnum;      (conference number)
/      int   itemnum;      (original item number)
/      Chix  itemchn;      (originating host of item)
/      int   freeze;       (1=>freeze the item; 0=>thaw it)
/      Chix  date;         (date item created, for XT_NEW)
/      Chix  time;         (time item created, for XT_NEW)
/      Chix  dest_chn;     (xaction destined for dest_chn)
/      int  *error;        (returned error code)
/
/   Purpose:
/      Send out a transaction to freeze or thaw a particular item.
/
/   How it works:
/      If FREEZE is 1, freeze the item.  If 0, thaw it.
/      Freeze or thaw item ITEMNUM, with original chn ITEMCHN,
/      in (local) conference number CONFNUM.
/
/      DATE and TIME may be NULL.
/      DEST_CHN is used by SEND_CONF() to send the xaction to the
/      supplying naybor.
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/
/   Error Conditions:
/
/   Related Functions: chgxresp, chgxtitle, chgxitem
/
/   Called by:
/
/   Home: xaction/frzxitem.c
/ */

/*: JV 12/04/91 14:13 Create this. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: CR 12/20/91 14:51 Realicize header comments. */
/*: JV 12/20/91 17:17 Add code for DEST_CHN. */
/*: CR 12/26/91 12:10 Call chxfree properly! */
/*: JV  1/14/92 13:21 Fix transit with valid dest_chn. */
/*: DE  6/03/92 13:17 Chixified */
/*: JX  7/31/92 14:12 Chixify DEST_CHN. */
/*: JX  9/01/92 13:05 Change empty_xact to make_xaction(). */

#include <stdio.h>
#include "chixuse.h"
#include "done.h"
#include "derr.h"
#include "null.h"
#include "handicap.h"
#include "unitcode.h"
#include "xaction.h"
#include "chixuse.h"
#include "goodlint.h"

extern union null_t null;

FUNCTION frzxitem (confnum, itemnum, itemchn, freeze, date, time,
                   dest_chn, error)
   int   confnum, itemnum, freeze, *error;
   Chix  itemchn, date, time;
   Chix  dest_chn;
{
   Chix  tname, locname, orighost, mychn, mydate, mytime; 
   int   lhnum, unit, success;
   int4  systime();
   char *strtoken();
   Xactionptr xact, make_xaction();
   Chix    carray[2];

   ENTRY ("frzxitem", "");

   tname     = chxalloc (L(80), THIN, "frzxitem tname");
   locname   = chxalloc (L(80), THIN, "frzxitem locname");
   orighost  = chxalloc (L(80), THIN, "frzxitem orighost");
   mychn     = chxalloc (L(80), THIN, "frzxitem mychn");
   mydate    = chxalloc (L(20), THIN, "frzxitem mydate");
   mytime    = chxalloc (L(20), THIN, "frzxitem mytime");
   success   = 0;

   xact = make_xaction();

   if (NOT get_chn (mychn, XSNF, 0)) {
      bug ("FRZXITEM: can't get XSNF.", 0); FAIL; }

   get_trueconf (confnum, locname, tname, orighost);

   xact->typenum = TT_ITEM;
   xact->subnum  = freeze ? SUBFREEZE : SUBTHAW;
   xact->actnum  = XT_CHANGE;
   if (NOT chxeq (orighost, mychn) && EMPTYCHX(dest_chn)) xact->transit = 1;
   chxcpy (xact->conftname, tname);
   chxcpy (xact->confochn,  orighost);
   xact->conflocnum = confnum;

   xact->itemid = itemnum;
   chxcpy (xact->itemchn, itemchn);

   if (EMPTYCHX(date)) {
      date=mydate; time=mytime;
      sysdaytime  (date, time, 0, systime());
   }
   if (chxlen (date) > 0) 
      chxformat (xact->date, CQ("%s %s"), L(0), L(0), date, time);

   carray[0] = carray[1] = chxalloc (L(0), THIN, "frzxitem carray");

   if (EMPTYCHX(dest_chn)) {  /* Normal xactions. */
      if (chxeq (mychn, orighost)) {
         unit = XCXT;
         if (NOT unit_lock (XCXT, WRITE, L(confnum), L(0), L(0), L(0), null.chx)) {
            bug ("FRZXITEM: can't lock XCXT.", 0); *error=DLOCK;
            chxfree (carray[0]); FAIL; }
      } else {
         unit = XHML;
         if (NOT get_course (orighost, &lhnum)) {
            bug ("FRZXITEM: host not found.", 0);  *error=DNOCHN;
            chxfree (carray[0]);  FAIL; }
         if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
            bug ("FRZXITEM: can't lock XHML.", 0); *error=DLOCK;
            chxfree (carray[0]);   FAIL; }
      }
   } else {                          /* SEND_CONF sending to one naybor. */
      unit = XHML;
      if ((lhnum = hnumber (dest_chn)) < 1) {
         bug ("FRZXITEM: bad DEST_CHN", 0); *error=DNOCHN;
         chxfree (carray[0]); FAIL; }
      if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
         bug ("FRZXITEM: can't lock XHML.", 0); *error=DLOCK;
         chxfree (carray[0]); FAIL; }
   }
   if (NOT unit_append (unit)) {
      bug ("FRZXITEM: can't open unit.", unit); *error=DFILE;
      unit_unlk (unit);  chxfree (carray[0]); FAIL; }
   putnextxact  (unit, xact, carray);
   unit_close   (unit);  unit_unlk (unit);

   chxfree (carray[0]);                   /* This frees both. */

   SUCCEED;
 done:
   chxfree (tname);
   chxfree (locname);
   chxfree (orighost);
   chxfree (mychn);
   chxfree (mydate);
   chxfree (mytime);
   free_xaction (xact);

   RETURN (success);
}   
