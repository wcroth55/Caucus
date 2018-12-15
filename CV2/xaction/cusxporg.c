/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CUSXPORG.  Create a "change primary organizer" transaction.
/
/   int n = cusxporg (confnum, new_porg_id, new_porg_chn, dest_chn, error)
/
/   Parameters:
/      int   confnum              (Local conf number)
/      Chix  new_porg_id          (Userid of new POrganizer)
/      Chix  new_porg_chn         (CHN of host where new POrganizer is)
/      Chix  dest_chn             (Destination CHN for SENDCONF)
/      int  *error
/
/   Purpose:
/      As with many cusx* and chgx* functions, cusxporg() has two purposes.
/      
/      To change the primary organizer of a conference, caucus_x calls this
/      to generate a xaction headed toward the host with the new porg for
/      this conf.
/      The XFU on the host of the new porg changes its DB and generates an
/      ACK of the request, and then the local XFU changes its DB.
/
/      The XFU calls this function when it is sending the entire conference
/      to a host which has successfully subscribed to the conf.  The XFU
/      uses the DEST_CHN argument to "direct" the xaction.
/
/      Neither of these affect the local Caucus DB in any way.
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
/   Home: xaction/cusxporg.c
/ */

/*: JV  6/10/91 18:21 Create this. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 14:41 Change Home, clean. */
/*: JV  7/09/91 15:12 Rewrite completely to use putnextxact() */
/*: JV  7/15/91 20:56 Add multiple text fields. */
/*: JV  7/22/91 16:34 Add ENTRY. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: JV  7/29/91 12:30 Add arg to get_trueconf */
/*: JV  8/29/91 15:30 Spell strcpy correctly */
/*: JV 11/06/91 10:29 Change common.h to null.h, add chixuse.h. */
/*: JV 11/12/91 12:06 Add confochn. */
/*: JV 11/13/91 21:53 Add DEST_CHN. */
/*: JV 11/18/91 14:31 Fix transit bug. */
/*: JV 12/07/91 19:40 Declare chxalloc. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: JV  1/03/92 15:17 Declare strcpy(). */
/*: DE  6/03/92 12:07 Chixified */
/*: JX  7/31/92 14:30 Chixify DEST_CHN. */
/*: JX  9/01/92 12:59 Change empty_xact to make_xaction(). */
/*: JX  9/22/92 14:31 Fixify. */
/*: JX 10/01/92 12:24 Undeclare sprintf. */

#include <stdio.h>
#include "chixuse.h"
#include "done.h"
#include "handicap.h"
#include "derr.h"
#include "null.h"
#include "unitcode.h"
#include "xaction.h"
#include "chixuse.h"

extern union null_t null;

FUNCTION cusxporg  (confnum, new_id, new_chn, dest_chn, error)
   int   confnum, *error;
   Chix  new_id, new_chn;
   Chix  dest_chn;
{
   Chix  tname, locname, orighost, mychn; 
   char *strcpy();
   int   lhnum, success;
   Chix  carray[2];
   Xactionptr xact, make_xaction();

   ENTRY ("cusxporg", "");

   tname    = chxalloc (L(80), THIN, "cusxporg tname");
   locname  = chxalloc (L(80), THIN, "cusxporg locname");
   orighost = chxalloc (L(80), THIN, "cusxporg orighost");
   mychn    = chxalloc (L(80), THIN, "cusxporg mychn");
   success  = 0;

   xact = make_xaction();

   if (NOT get_chn (mychn, XSNF, 0)) {
      bug ("cusxporg: couldn't get XSNF.", 0); FAIL; }

   get_trueconf (confnum, locname, tname, orighost);

   xact->typenum = TT_CONFERENCE;
   xact->subnum  = SUBPORG;
   xact->actnum  = XT_CHANGE;
   if (EMPTYCHX (dest_chn))  xact->transit = 1;
   chxcpy (xact->conftname, tname);
   chxcpy (xact->confochn,  orighost);
   xact->conflocnum = confnum;
   chxformat (xact->uid, CQ("%s@%s"), L(0), L(0), new_id, new_chn);
/* sprintf (xact->date, "%s %s", date, time); */

   if (EMPTYCHX (dest_chn)) {
      if (NOT get_course (new_chn, &lhnum)) {
         bug ("CUSXPORGANIZER: host not found.", 0);
         *error=DNOCHN; FAIL; }
      if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
         bug ("CUSXPORGANIZER: can't lock XHML.", 0);
         *error=DLOCK;  FAIL; }
      if (NOT unit_append (XHML)) {
         bug ("CUSXPORGANIZER: can't open XHML.", 0); *error=DFILE;
         unit_unlk (XHML);  FAIL; 
      }
   } else {
      if ((lhnum = hnumber (dest_chn)) < 1) {
         bug ("CUSXPORGANIZER: couldn't find neighbor.", 0);
         *error = DBADDB;  FAIL; }
      if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
         bug ("CUSXPORGANIZER: can't lock XHML.", 0);
         *error=DLOCK;  FAIL; }
      if (NOT unit_append (XHML)) {
         bug ("CUSXPORGANIZER: can't open XHML.", 0); *error=DFILE;
         unit_unlk (XHML);   FAIL;
      }
   }

   carray[0] = carray[1] = chxalloc (L(0), THIN, "cusxporg carray");
   putnextxact (XHML, xact, carray);
   chxfree (carray[0]);
   unit_close (XHML);  unit_unlk (XHML);

   SUCCEED;
 done:
   chxfree (tname);
   chxfree (locname);
   chxfree (orighost);
   chxfree (mychn);
   free_xaction (xact);

   RETURN (success);
}   
