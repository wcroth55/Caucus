/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** Addxosubject.  Create or change an OSUBJECT.
/
/   int n = addxosubject (type, confnum, title, changes, dest_chn, error)
/
/   Parameters:
/      int   type;          (XT_NEW, XT_CHANGE, or XT_DELETE)
/      int   confnum;       (local conference number)
/      Chix  title;         (full name of subject)
/      Chix  changes;       (new data for subject, if XT_CHANGE)
/      Chix  dest_chn;      (destination of osubject transactions)
/      int  *error;         (put returned error code here)
/
/   Purpose:
/      Caucus_x should call this to generate a xaction headed toward
/      the original host so that this osubject gets added or changed
/      on all the other appropriate hosts.  TYPE indicates whether this
/      is NEW, CHANGE, or DELETE.
/
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
/   Home: xaction/addxosub.c
/ */

/*: JV  6/10/91 16:46 Create this. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/09/91 13:43 Rewrite completely to use putnextxact() */
/*: JV  7/15/91 19:46 Add multiple text fields. */
/*: JV  7/22/91 16:32 Add ENTRY */
/*: CR  7/27/91 14:17 Include "goodlint.h". */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: JV  7/29/91 12:18 Change get_trueconf call. */
/*: JV  7/31/91 11:54 Close XCNB when done, only free chx once. */
/*: JV 10/30/91 11:26 Add optional destination CHN for xactions. */
/*: JV 11/06/91 10:29 Changed common.h to null.h. */
/*: JV 11/11/91 14:30 Allow CHN==null.str, add confochn. */
/*: JV 11/18/91 13:15 Fix bugs. */
/*: JV 11/26/91 13:32 Send chg's generated at OCHN to XCXT, not XHML. */
/*: JV 12/07/91 19:24 Declare chxalloc. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: JV  1/17/92 16:20 Remove old deadcode. */
/*: CX  6/02/92 12:33 Rearrange include files. */
/*: DE  6/02/92 13:42 Chixified */
/*: JX  7/31/92 14:00 Chixify DEST_CHN. */
/*: JX  9/01/92 12:36 Change empty_xact to make_xaction(). */
/*: JX 10/01/92 12:23 Undeclare sprintf. */
/*: CR  2/12/93 13:02 Fix header comments. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "derr.h"
#include "null.h"
#include "unitcode.h"
#include "xaction.h"
#include "goodlint.h"
#include "done.h"

#define LEAVE goto ERRORPROC

extern union null_t null;

FUNCTION addxosubject (type, confnum, title, changes, dest_chn, error)
   int   type, confnum, *error;
   Chix  dest_chn;
   Chix  title, changes;
{
   Chix tname, locname, orighost, mychn, subscriber, userid;
   char temp[120];
   int  lhnum, unit, success;
   Chix carray[TEXTFIELDS];
   Xactionptr xact, make_xaction();

   ENTRY ("addxosubject", "");

   tname      = chxalloc (L(80),  THIN, "addxosub tname");
   locname    = chxalloc (L(80),  THIN, "addxosub locname");
   orighost   = chxalloc (L(80),  THIN, "addxosub orighost");
   mychn      = chxalloc (L(80),  THIN, "addxosub mychn");
   subscriber = chxalloc (L(80),  THIN, "addxosub subscriber");
   userid     = chxalloc (L(80),  THIN, "addxosub userid");   
   success    = 0;

   xact =  make_xaction();

   if (NOT get_chn (mychn, XSNF, 0)) {
      bug ("ADDXOSUBJECT: couldn't get XSNF.", 0); FAIL; }

   get_trueconf (confnum, locname, tname, orighost);

   xact->typenum = TT_OSUBJECT;
   xact->actnum  = type;
   if ((type==XT_CHANGE || type==XT_DELETE) && NOT chxeq (orighost, mychn) &&
       (EMPTYCHX (dest_chn) ))
      xact->transit = 1;
   chxcpy (xact->conftname, tname);
   chxcpy (xact->confochn,  orighost);
   xact->conflocnum = confnum;
   chxcpy (xact->title, title);
/* if (date[0] != '\0') sprintf (xact->date, "%s %s", date, time); */

   switch (type) {
   case XT_NEW:
      if (EMPTYCHX (dest_chn)) { /* Normal NEW xactions.*/
         unit = XUXF;
         sysuserid (userid);
         if (NOT unit_lock (XUXF, WRITE, L(0), L(0), L(0), L(0), userid)) {
            bug ("ADDXOSUBJECT: can't lock XUXF.", 0); *error=DLOCK;
            FAIL; }
      } else {                                    /* NEW CONF request */
         unit = XHML;
         if ((lhnum = hnumber (dest_chn)) < 1) {
            sprintf (temp,"ADDXOSUBJECT: sending to bad host name: %s.",
                     ascquick (dest_chn));
            bug (temp, 0); }
         if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
            bug ("ADDXOSUBJECT: can't lock XHML.", 0); *error=DLOCK;
            FAIL; }
      }

      if (NOT unit_append (unit)) {
         bug ("ADDXOSUBJECT: can't open unit: ", unit); *error=DFILE;
         unit_unlk (unit); FAIL; }

      carray[0] = carray[1] = chxalloc (L(0), THIN, "addxosub carray");
      putnextxact (unit, xact, carray);
      chxfree (carray[0]);
      unit_close  (unit); unit_unlk (unit);
      break;

   case XT_CHANGE:
   case XT_DELETE:
   if (type == XT_CHANGE) {
      xact->datasize[0] = chxlen (changes);
      carray[0] = changes;
      carray[1] = chxalloc (L(0), THIN, "addxosub carray[1]");
   } else
      carray[0] = carray[1] = chxalloc (L(0), THIN, "addxosub carray");

   /*** If we're the Original host, send down the tree. */
   if (chxeq (orighost, mychn) && EMPTYCHX (dest_chn)) {
      if (NOT unit_lock (XCXT, WRITE, L(confnum), L(0), L(0), L(0), null.chx)) {
         bug ("ADDXOSUB: can't lock XCXT.", 0); *error=DLOCK; LEAVE; }
      if (NOT unit_append (XCXT)) {
         bug ("ADDXOSUB: can't open XCXT.", 0);
         unit_unlk (XCXT);
         *error=DFILE;  LEAVE; }
      putnextxact (XCXT, xact, carray);
      unit_close (XCXT); unit_unlk (XCXT);
      chxfree (carray[1]);                /* Frees both if XT_DELETE */
      break;

   /*** If we're not the OCHN, but this is a "user change", send up the tree
          */
   } else if (EMPTYCHX (dest_chn)) {
      if (NOT get_course (orighost, &lhnum)) {
         bug ("ADDXOSUBJECT: host not found.", lhnum);
         *error=DNOCHN; LEAVE; }
      if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
         bug ("ADDXOSUBJECT: can't lock XHML.", 0); *error=DLOCK;  LEAVE; }
      if (NOT unit_append (XHML)) {
         bug ("ADDXOSUBJECT: can't open XHML.", 0); *error=DFILE;
         unit_unlk (XHML);  LEAVE; }
      putnextxact (XHML, xact, carray);
      chxfree (carray[1]);                      /* Frees both if XT_DELETE */
      unit_close (XHML); unit_unlk (XHML);
      break;

   /*** We're any host and being called by SEND_CONF(). */
   } else {
      if ((lhnum = hnumber (dest_chn)) < 1) {
         sprintf (temp, "CHGXITEM: sending to bad host name: %s.",
                  ascquick (dest_chn));
         bug (temp, 0); }
      if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
            bug ("CHGXITEM: can't lock XHML.", 0); *error=DLOCK;
            LEAVE; }
      if (NOT unit_append (XHML)) {
         bug ("ADDXOSUBJECT: can't open XHML.", 0); *error=DFILE;
         unit_unlk (XHML);  LEAVE; }
      putnextxact (XHML, xact, carray);
      chxfree (carray[1]);                      /* Frees both if XT_DELETE */
      unit_close (XHML); unit_unlk (XHML);
      break;
   }
   default:
      bug ("addxosubject: bad type.", 0); FAIL;
   } /* switch */

   SUCCEED;

ERRORPROC:
   chxfree (carray[1]);
   success = 0;
 done:
   chxfree (tname);
   chxfree (locname);
   chxfree (orighost);
   chxfree (mychn);
   chxfree (subscriber);
   chxfree (userid);
   free_xaction (xact);

   RETURN (success);
}
