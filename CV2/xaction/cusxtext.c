/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CUSXTEXT.  Create a CUSXTEXT transaction.
/
/   int n = cusxtext (confnum, which, text, dest_chn, error)
/
/   Parameters:
/      int     confnum;     (conference number)
/      int     which;       (which text: CUS_GREET or CUS_INTRO)
/      Chix    text;        (text being changed)
/      Chix    dest_chn;    (Destination CHN for SENDCONF)
/      int    *error;       (returned error code)
/
/   Purpose:
/      Caucus_x should call this to generate a xaction headed toward the
/      original host for this conf, to eventually change the text of the
/      conference greeting or conference introduction.
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
/   Home: xaction/cusxtext.c
/ */

/*: JV  6/10/91 18:21 Create this. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/04/91 18:04 Remove lint. */
/*: JV  7/09/91 16:26 Rewrite completely with putnextxact() */
/*: JV  7/15/91 19:27 Add multiple text fields. */
/*: JV  7/22/91 16:35 Add ENTRY */
/*: CR  7/27/91 14:17 Include "goodlint.h". */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: JV  7/29/91 12:31 Add arg to get_trueconf */
/*: JV 11/06/91 10:29 Change common.h to null.h. */
/*: JV 11/12/91 12:06 Add confochn. */
/*: CR 11/05/91 19:17 Make header more informative. */
/*: JV 11/13/91 22:17 Add DEST_CHN. */
/*: JV 11/15/91 12:09 Chg hnumber() to strnum() for subscribers. */
/*: JV 11/18/91 14:32 Fix transit bug. */
/*: JV 11/26/91 13:32 Send chg's generated at OCHN to XCXT, not XHML. */
/*: JV 12/07/91 19:41 Declare chxalloc. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: JV  1/03/92 15:00 Remove unused code and var's. */
/*: DE  6/03/92 12:19 Chixified */
/*: JX  7/31/92 14:10 Chixify DEST_CHN. */
/*: JX  9/01/92 13:01 Change empty_xact to make_xaction(). */
/*: JX  9/22/92 14:31 Fixify. */

#include <stdio.h>
#include "chixuse.h"
#include "done.h"
#include "handicap.h"
#include "derr.h"
#include "null.h"
#include "unitcode.h"
#include "chixuse.h"
#include "xaction.h"
#include "goodlint.h"

extern union null_t  null;

FUNCTION cusxtext (confnum, which, ctext, dest_chn, error)
   int   confnum, which, *error;
   Chix  ctext;
   Chix  dest_chn;
{
   Chix tname, locname, orighost, mychn;
   int  lhnum, success;
   Chix carray[2];
   Xactionptr xact, make_xaction();

   ENTRY ("cusxtext", "");

   tname     = chxalloc (L(80), THIN, "cusxtext tname");
   locname   = chxalloc (L(80), THIN, "cusxtext locname");
   orighost  = chxalloc (L(80), THIN, "cusxtext orighost");
   mychn     = chxalloc (L(80), THIN, "cusxtext mychn");
   success   = 0;

   xact = make_xaction();

   if (NOT get_chn (mychn, XSNF, 0)) {
      bug ("cusxtext: couldn't get XSNF.", 0); FAIL; }

   get_trueconf (confnum, locname, tname, orighost);

   xact->typenum = TT_CONFERENCE;
   xact->subnum  = which;
   xact->actnum  = XT_CHANGE;
   if (NOT chxeq (orighost, mychn) && (EMPTYCHX(dest_chn))) xact->transit = 1;
   chxcpy (xact->conftname, tname);
   chxcpy (xact->confochn,  orighost);
   xact->conflocnum = confnum;
   xact->datasize[0] = chxlen (ctext);

   if (NOT EMPTYCHX (dest_chn)) {
      if ((lhnum = hnumber (dest_chn)) < 1) {
         bug ("CUSXTEXT: couldn't find neighbor.", 0);
         *error = DBADDB;  FAIL; }
      if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
         bug ("CUSXTEXT: can't lock XHML.", 0); *error=DLOCK;  FAIL; }
      if (NOT unit_append (XHML)) {
         bug ("CUSXTEXT: can't open XHML.", 0); *error=DFILE;
         unit_unlk (XHML);   FAIL;
      }
      carray[0] = ctext;
      carray[1] = chxalloc (L(0), THIN, "cusxtext carray[1]");
      putnextxact (XHML, xact, carray);
      chxfree (carray[1]);
      unit_close (XHML);  unit_unlk (XHML);

   } else if (chxeq (mychn, orighost)) {
      if (NOT unit_lock (XCXT, WRITE, L(confnum), L(0), L(0), L(0), null.chx)) {
         bug ("CUSXTEXT: can't lock XCXT.", 0); *error=DLOCK; FAIL; }
      if (NOT unit_append (XCXT)) {
         bug ("CUSXTEXT: can't open XCXT.", 0);
         unit_unlk (XCXT);
         *error=DFILE;  FAIL; }
      carray[0] = ctext;
      carray[1] = chxalloc (L(0), THIN, "cusxtext carray[1]");
      putnextxact (XCXT, xact, carray);
      unit_close (XCXT); unit_unlk (XCXT);
      chxfree (carray[1]);

   } else {
      if (NOT get_course (orighost, &lhnum)) {
         bug ("CUSXTEXT: host not found.", 0);  *error=DNOCHN; FAIL; }
      if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
         bug ("CUSXTEXT: can't lock XHML.", 0); *error=DLOCK;  FAIL; }
      if (NOT unit_append (XHML)) {
         bug ("CUSXTEXT: can't open XHML.", 0); *error=DFILE;
         unit_close (XHML);  FAIL; 
      }
      carray[0] = ctext;
      carray[1] = chxalloc (L(0), THIN, "cusxtext carray[1]");
      putnextxact (XHML, xact, carray);
      chxfree (carray[1]);
      unit_close (XHML);  unit_unlk (XHML);
   }
   SUCCEED;
 done:
   chxfree (tname);
   chxfree (locname);
   chxfree (orighost);
   chxfree (mychn);
   free_xaction (xact);

   RETURN (success);
}   

