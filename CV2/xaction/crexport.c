/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/* CREXPORT.  Create a new transport file from a xaction file.
/
/  n = crexport (lhnum, uxact, newheader, confnum, pheader);
/
/  Parameters:
/     int lhnum, uxact, newheader, confnum;
/     XPORTHDR *pheader;
/
/  Purpose:
/    There are three types of transaction files: conf (XCXT, XHXA)
/    and mail (XHML).  At some point we need to convert each file
/    of these types into xport files.  That's what crexport is for.
/
/    Actually, a "transport file" only exists just prior to/during
/    transit, and in the receiving bay.  Most of the time before
/    shipment it sits in the shipping bay in two pieces:
/    1) a "transport header-file" (XPXH) which contains information
/       *about* the transactions in this xport file
/    2) a "transport text-file" (XPXT) which contains *just* the xactions
/
/    Crexport creates these two files.
/
/    "Converting" them means one of two things:
/    1) For XCXT and XHML, crexport adds a xport header to each
/    2) For XHXA, which already has a header (the one used to get
/       to this host), crexport changes the header to reflect the
/       new destination.
/
/  How it works:
/     LHNUM is the local host number that we'll be using to write
/     out the transport files and possibly to find the xaction file.
/     UXACT is the unit number for the xactions we want to use.
/     CONFNUM is the local conf num if we're processing XCXT.
/     NEWHEADER should be 1 or 0.  If it's 1, crexport should create
/     a new header.  If it's 0 (which implies XHXA), there already is a
/     header, which is passed in thru PHEADER, but the destination
/     needs to be changed to the next host we should be sending
/     the xport file to.
/
/     First crexport gets the status of the attachment to that LHNUM,
/     and figures out what shipping method to use.  Using these, it
/     uses very simple unit_* routines to create the xport header-file
/     and the xport text-file.
/
/     Note: since for XHXA we need to lock two HOSTINFO files at the same time
/     (the host this file came FROM and each of the ones it's going TO)
/     we have to figure out which one to use.  This is done very early
/     on.
/
/     Crexport() assumes that the caller has locked and opened,
/     and will close and unlock, unit number UXACT since the 
/     xactions may come from one of three different places.
/
/     Assumes XHI1 (or XHI2) for this neighbor is locked for WRITE.
/
/  Warning: this code does not take care of the broadcast counter yet.
/
/  Returns:
/     1 on success, 0 on failure.
/
/  Error Conditions
/
/  Related Functions:
/  
/  Called by:
/     xp_ship(), mailxp_ship(), xpthrush()
/
/  Home:xaction/crexport.c
*/

/*: JV  4/06/91  0:17 Create function. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 18:00 Change get_chn call. */
/*: JV  7/04/91 17:55 Remove lint. */
/*: JV  7/17/91 10:36 Fix XHI# bug. */
/*: JV  7/22/91 16:33 Add ENTRY. */
/*: JV  7/29/91 12:28 Add arg to get_trueconf, define hinfo. */
/*: JV  8/07/91 09:20 Add trueconf field to XHXA. */
/*: JV  8/09/91 12:52 Add 2nd arg to set_neighbor. */
/*: JV  9/19/91 09:18 Use seqnum macro ops. */
/*: JV 10/14/91 10:06 Change OCHN on forwards. */
/*: JV 10/17/91 17:34 Add audit trail, fix destchn. */
/*: JV 10/31/91 13:46 Clean. */
/*: JV 11/06/91 10:29 Change common.h to null.h. */
/*: JV 11/18/91 11:15 Add code to handle header.confochn. */
/*: JV 12/07/91 19:36 Deallocate chixen. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: JV 12/21/91 13:49 Allow xactions to go to PENDING naybors. */
/*: CR 12/21/91 17:45 Status check must be AND, not OR. */
/*: CR 12/23/91 23:24 Longs must have %ld in sprintf(). */
/*: JV  1/03/92 14:57 Remove unused code and var's. */
/*: JV  1/20/92 14:48 Get conf xactions from XCUX instead of XCXT. */
/*: DE  6/03/92 11:29 Chixified */
/*: JX  7/24/92 15:30 Convert to make_hinfo(). */
/*: JX  9/01/92 14:03 Allocate xact, header, fields. */
/*: JX  9/30/92 17:31 Clear xact between getnextxact() calls. */

#include <stdio.h>
#include "chixuse.h"
#include "done.h"
#include "handicap.h"
#include "goodlint.h"
#include "null.h"
#include "unitcode.h"
#include "xaction.h"
#include "audit.h"
#include "xfp.h"
#include "sequence.h"

#define END(x) unit_close (x); unit_unlk (x)

extern Chix cnetvers;

FUNCTION crexport (lhnum, uxact, newheader, confnum, pheader)
   int lhnum, uxact, newheader, confnum;
   XPORTHDR *pheader;
{
   Chix  locname;
   char  buf[8], *strito36();
   int  i, success;
   Xactionptr  xact, make_xaction();
   XPORTHDR *header, *make_xporthdr();
   Hinfoptr make_hinfo ();
   HOSTPTR  host, make_host();
   Chix     carray[TEXTFIELDS];

   ENTRY ("crexport", "");

   for (i=0; i < TEXTFIELDS; i++)
      carray[i] = chxalloc (L(100), THIN, "moveu carray1");
   locname  = chxalloc (L(80), THIN, "crexport locname");
   xact = make_xaction();
   host = make_host();
   host->info = make_hinfo();

   header = make_xporthdr();
   success  = 0;

   if (NOT get_neighbor (lhnum, host, XHI1)) {
      bug ("CREXPORT: not a neighbor: ", lhnum); FAIL; }

   /*** Make sure that the naybor is attached.  The only xaction that
   /    can be generated while status==PENDING is ATTREQ. */
   if (host->info->status != ATTACHED  &&  host->info->status != PENDING) {
      bug ("CREXPORT: attempt to send to naybor that's not ATTACHED.", 0);
      FAIL; }

   if (NOT unit_lock (XPXH, WRITE, L(host->info->shipmeth), L(lhnum), 
                      host->info->seqnum_to_hi, L(0), nullchix)) {
      bug ("CREXPORT: can't lock transport header-file: ", 0);
      FAIL; }
   if (NOT unit_lock (XPXT, WRITE, L(host->info->shipmeth), L(lhnum),
                      host->info->seqnum_to_hi, L(0), nullchix)) {
      bug ("CREXPORT: can't lock transport text-file: ", 0);
      unit_unlk (XPXH); FAIL; }
  
   unit_make  (XPXH);  unit_make (XPXT);

   strito36 (buf, host->info->seqnum_to_hi, 5);
   printf ("Packing xport file: SHIP%03d/J%03d%4s.\n",
            host->info->shipmeth, lhnum, buf);
   /* If we need to, first write the xport header to the xport file. */
   if (newheader) {
      strcpy  (header->version, ascquick(cnetvers));
      get_chn (header->origchn, XSNF, 0);
      chxcpy  (header->destchn, host->chn);
/*    get_chn (header->destchn, XHI2, lhnum); */
      strcpy  (header->broadcntr, "");       /*  This needs to be fixed. */

      /*** Here's where we use and increment the sequence # for this file. */
      sprintf (header->nayseq, "%d", host->info->seqnum_to_hi);
      host->info->seqnum_to_hi = INC(host->info->seqnum_to_hi, SEQ_TO_MOD);

      /*** Type depends on the source of the xactions. */
      switch (uxact) {
      case (XCUX):
         strcpy  (header->type, "CONF");
         header->typenum = XCONF;
         get_trueconf (confnum, locname, header->trueconf, header->confochn);
         break;
      case (XHML):
         strcpy  (header->type, "MSGS");
         header->typenum = XMSGS;         break;
      case (XHXA):
         bug ("CREXPORT: Called for XHXA with NEWHEADER.", 0);
         END (XPXH); END (XPXT); FAIL;
      default:
         bug ("CREXPORT: Called with bad unit number:", uxact);
         END (XPXH); END (XPXT); FAIL;
      }
      set_xpheader (XPXH, header);
   } else {
      /* Otherwise it's an XHXA, so get the header, change the orig &
      /  dest CHN and sequence #, and write the new header out. */
      chxcpy  (pheader->destchn, host->chn);
      get_chn (pheader->origchn, XSNF, 0);
      sprintf (pheader->nayseq, "%d", host->info->seqnum_to_hi);
      host->info->seqnum_to_hi = INC(host->info->seqnum_to_hi, SEQ_TO_MOD);
      get_trueconf (confnum, locname, pheader->trueconf, pheader->confochn);
      set_xpheader (XPXH, pheader);
   }

   unit_write (XPXH, CQ("\n"));    /* Separator between header and text. */

   /* Read and write xactions, but skip blank line(s) at end of header. */
   while (getnextxact (uxact, xact)) {
      for (i=0; i < TEXTFIELDS; i++)
         chxcpy (carray[i], xact->data[i]);

      audit_xaction (xact, lhnum, AUDIT_OUT);
      putnextxact (XPXT, xact, carray);

      clear_xaction (xact);
      for (i=0; i < TEXTFIELDS; i++) chxclear (carray[i]);
   }

   unit_close (XPXH);    unit_unlk (XPXH);
   unit_close (XPXT);    unit_unlk (XPXT);
   
   set_neighbor (host, XHI1);     /* Update the sequence number. */
   SUCCEED;

 done:
   chxfree (locname);
   for (i=0; i < TEXTFIELDS; i++) chxfree (carray[i]);
   free_host     (host);
   free_xaction  (xact);
   free_xporthdr (header);
   
   RETURN (success);
}
