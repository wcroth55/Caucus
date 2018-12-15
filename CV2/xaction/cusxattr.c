/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CUSXATTR.  Create a Customize Attribute transaction.
/
/   int n = cusxattr (confnum, attr, value, dest_chn, error)
/
/   Parameters:
/      int   confnum      (Local conf number)
/      int   attr         (Which attribute (see below))
/      int   value        (New value of attribute)
/      Chix  dest_chn     (Destination CHN for SENDCONF)
/      int  *error
/
/   Purpose:
/      Caucus_x should call this to generate a xaction headed toward the
/      original host for this conf, to eventually change an attribute.
/      It does not affect the local Caucus DB in any way.
/
/   How it works:
/      ATTR must be one of:
/         CUS_ADD      CUS_SUBJECT      CUS_ATTACH
/         CUS_CHANGE   CUS_VISIBILITY   CUS_GREET  CUS_INTRO
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
/   Home: xaction/cusxattr.c
/ */

/*: JV  6/10/91 18:21 Create this. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 19:15 Clean. */
/*: JV  7/09/91 14:57 Rewrite completely to use putnextxact() */
/*: CR  7/16/91 21:34 JV made unknown changes since last carve. */
/*: JV  7/22/91 16:34 Add ENTRY. */
/*: CR  7/26/91 18:23 chxofasc() is really chxofascii(). */
/*: CR  7/27/91 14:17 Include "goodlint.h". */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: JV  7/29/91 12:29 Add arg to get_trueconf */
/*: JV 11/06/91 10:29 Change common.h to null.h, add chixuse.h. */
/*: JV 11/12/91 12:05 Add confochn. */
/*: JV 11/13/91 22:09 Add DEST_CHN. */
/*: JV 11/15/91 12:08 Chg hnumber() to strnum() for subscribers. */
/*: JV 11/18/91 14:30 Fix transit bug. */
/*: JV 11/26/91 13:32 Send chg's generated at OCHN to XCXT, not XHML. */
/*: JV 12/07/91 19:37 Declare chxalloc(). */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: CR  1/02/92 15:06 Add CUS_SUBJECT to comments. */
/*: JV  1/03/92 14:59 Remove unused code and var's. */
/*: DE  6/03/92 11:50 Chixified */
/*: JX  7/31/92 14:05 Chixify DEST_CHN. */
/*: JX  9/01/92 12:56 CHange empty_xact make_xaction(). */
/*: JX  9/22/92 14:31 Fixify. */
/*: JV  2/24/93 17:02 Add CUS_ATTACH. */

#include <stdio.h>
#include "chixuse.h"
#include "done.h"
#include "handicap.h"
#include "derr.h"
#include "null.h"
#include "unitcode.h"
#include "xaction.h"
#include "goodlint.h"

extern union null_t null;

FUNCTION cusxattr (confnum, attr, value, dest_chn, error)
   int   confnum, attr, value, *error;
   Chix  dest_chn;
{
   Chix tname, locname, orighost, mychn;
   int  lhnum, success;
   Chix carray[2];
   Xactionptr xact, make_xaction();

   ENTRY ("cusxattr", "");

   tname     = chxalloc (L(80), THIN, "cusxattr tname");
   locname   = chxalloc (L(80), THIN, "cusxattr locname");
   orighost  = chxalloc (L(80), THIN, "cusxattr orighost");
   mychn     = chxalloc (L(80), THIN, "cusxattr mychn");
   success   = 0;

   xact = make_xaction();

   if (NOT get_chn (mychn, XSNF, 0)) {
      bug ("cusxattr: couldn't get XSNF.", 0); FAIL; }

   get_trueconf (confnum, locname, tname, orighost);

   xact->typenum = TT_CONFERENCE;
   xact->subnum  = attr;
   xact->actnum  = XT_CHANGE;
   if (NOT chxeq (orighost, mychn) && (EMPTYCHX(dest_chn))) xact->transit = 1;
   chxcpy (xact->conftname, tname);
   chxcpy (xact->confochn,  orighost);
   xact->conflocnum = confnum;
   xact->datasize[0] = 3 + (1-value);    /* ON\n=3, OFF\n=4 */

   if (NOT EMPTYCHX (dest_chn)) {
      if ((lhnum = hnumber (dest_chn)) < 1) {
         bug ("CUSXATTR: couldn't find neighbor.", 0);
         *error = DBADDB;  FAIL; }
      if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
         bug ("CUSXATTR: can't lock XHML.", 0); *error=DLOCK;  FAIL; }
      if (NOT unit_append (XHML)) {
         bug ("CUSXATTR: can't open XHML.", 0); *error=DFILE;
         unit_unlk (XHML);   FAIL;
      }
      carray[0] = chxalloc (L(4), THIN, "cusxattr carray[0]");
      chxofascii (carray[0], value ? "ON\n" : "OFF\n");
      carray[1] = chxalloc (L(0), THIN, "cusxattr carray[1]");
      putnextxact (XHML, xact, carray);
      chxfree (carray[1]); chxfree (carray[0]);
      unit_close (XHML);  unit_unlk (XHML);

   } else if (chxeq (mychn, orighost)) {
      if (NOT unit_lock (XCXT, WRITE, L(confnum), L(0), L(0), L(0), null.chx)) {
         bug ("CUSXATTR: can't lock XCXT.", 0); *error=DLOCK; FAIL; }
      if (NOT unit_append (XCXT)) {
         bug ("CUSXATTR: can't open XCXT.", 0);
         unit_unlk (XCXT);
         *error=DFILE;  FAIL; }
      carray[0] = chxalloc (L(4), THIN, "cusxattr carray[0]");
      chxofascii (carray[0], value ? "ON\n" : "OFF\n");
      carray[1] = chxalloc (L(0), THIN, "cusxattr carray");
      putnextxact (XCXT, xact, carray);
      unit_close (XCXT); unit_unlk (XCXT);
      chxfree (carray[0]); chxfree (carray[1]);

   } else {
      if (NOT get_course (orighost, &lhnum)) {
         bug ("CUSXATTR: host not found.", 0);  *error=DNOCHN; FAIL; }
      if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
         bug ("CUSXATTR: can't lock XHML.", 0); *error=DLOCK;  FAIL; }
      if (NOT unit_append (XHML)) {
         bug ("CUSXATTR: can't open XHML.", 0); *error=DFILE;
         unit_unlk (XHML);  FAIL;
      }
      carray[0] = chxalloc (L(4), THIN, "cusxattr carray[0]");
      chxofascii (carray[0], value ? "ON\n" : "OFF\n");
      carray[1] = chxalloc (L(0), THIN, "cusxattr carray");
      putnextxact (XHML, xact, carray);
      chxfree (carray[0]); chxfree (carray[1]);
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

