/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** GET_NEIGHBOR.   Get all the information about a neighbor from 
/                    XHI1 or XHI2, or about this host from XSNF.
/
/   int n = get_neighbor (lhnum, hostptr, unit)
/
/   Parameters:
/      int lhnum, unit;
/      HOSTPTR hostptr;
/
/   Purpose:
/      Frequently we need the following pieces of information about
/      a neighboring host:
/      1) CHN        2) LHName      3) LHNumber
/      4) Caucus ID of Caucus Mgr of Neighbor
/      5) Shipping method to use to send files
/      6) Status of Connection (Attached, Attached Pending, etc.)
/      7) Various sequence numbers used to coordinate shipping/receiving
/      Get_neighbor fills in a struct of type HOST with this info.
/
/      If LHNUM is 0, get_neighbor fills in the HOST struct from XSNF.
/      In this case, fields that have no meaning are stored as zeroes.
/      This includes ShipMethod#, Status, and all of the Seq#'s except
/      the first, second, fifth, and sixth, which are used as the upper
/      and lower bounds for the active XSPB's and XSRP's, respectively.
/      In the case of XSRP (the ReParseBay) the upper number refers to the
/      file we're currently filling.  If we empty that file, both the upper
/      and lower bounds should be set one number *higher* than that.
/
/      Get_neighbor() can get the info from either XHI1 or XHI2 (or XSNF).
/
/   How it works:
/      Get_neighbor uses unit_read to read in each of the lines of
/      XHI1 or XHI2.
/      It has to translate some of the lines into int's.
/
/      This version of get_neighbor is (slightly) "object-oriented" in that 
/      it loads the information about *this* host (XSNF) on the first
/      such request.  During this load it also copies the information
/      into static memory so it doesn't have to go to disk next time.
/      We can do this because the information in XSNF isn't likely to
/      change except for calls to set_neighbor().  If set_neighbor
/      is called on XSNF, it flips XSNF_FIRST back to 1 (one) so that
/      the next call to get_neighbor will reload the host info.
/
/      Get_neighbor assumes that XHI1, XHI2, or XSNF was locked by
/      the caller and will be unlocked by the caller.  This is so
/      that the caller can lock XHI1 during a sequence number update.
/
/      Get_neighbor also assumes that the caller has allocated all of the
/      necessary memory (HOST and Hostinfo structures).  This is so that
/      the caller can free them when it's done.
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/
/   Error Conditions:
/      Get_neighbor checks occasionally as it's reading in information
/      to make sure the data it's reading makes sense.
/
/   Related Functions: set_neighbor()
/
/   Called by: crexport(),  xfu (Main)
/
/   Home: xaction/getneigh.c
/ */

/*: JV  5/02/91 10:40 Create this. */
/*: JV  7/04/91 17:56 Remove lint. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/05/91 10:17 Add entryex.h */
/*: JV  7/16/91 13:51 Add reparse_num */
/*: JV  7/19/91 12:25 Help heap. */
/*: JV  7/29/91 18:09 Don't need to sysmem for things in HOST anymore. */
/*: JV  8/07/91 09:56 Return after copying from cache. */
/*: CR  8/08/91 21:58 Handle new tosstime,reshiptime members. */
/*: CR  8/13/91 11:55 Use strscan() in place of strnum(). */
/*: CR  8/20/91 21:28 Add ACKSEQ, keep_chunks members. */
/*: JV  8/26/91 16:32 Move keep_chunks to the right place. */
/*: JV  2/03/92 11:03 Don't cache XSNF. */
/*: DE  6/03/92 16:17 Chixified */
/*: DE  6/08/92 15:03 Fix Chix */
/*: JX  7/24/92 15:35 Remove strscan(), use make_hinfo(). */
/*: JX  8/24/92 16:10 Change most chxnum's to chxint4's, SUCCEED at end. */
/*: CR 10/09/92 17:48 Comment out copyhost() defn. */
/*: CR 12/11/92 15:37 Long chxint4 arg. */
/*: CL 12/11/92 15:52 Long sysmem arg. */

#include <stdio.h>
#include "handicap.h"
#include "goodlint.h"
#include "entryex.h"
#include "unitcode.h"
#include "xfp.h"
#include "done.h"

int xsnf_first=1;

FUNCTION get_neighbor (lhnum, hostptr, unit)
int lhnum, unit;
HOSTPTR hostptr;
{
   static HOSTPTR  myhost;
   static int first_time = 1;
   Hostinfo *hinfo;
   HOSTPTR  make_host();
   Hinfoptr make_hinfo();
   Chix   line, s;
   int    n, success;
   int4   pos;

   ENTRY ("get_neighbor", "");

   if (first_time) {
      myhost = make_host();
      myhost->info = make_hinfo();
      first_time = 0;
    }

   line     = chxalloc (L(100), THIN, "getneigh line");
   success  = 0;
   pos = 0; 

   if (unit != XSNF && lhnum == 0) {
      bug ("GET_NEIGHBOR: no host number.", 0); FAIL;
   }
   n = unit;

/*** Be careful with caching info that could change in the file:
/    you might overwrite new data (saved by another process) with
/    old data (that was cahced by this process). 
/  if (lhnum != 0) n = unit;        Only load XSNF the first time. 
/  else { 
/     n = XSNF;
/     if (NOT xsnf_first) {
/        copyhost (hostptr, myhost);
/        RETURN (1); }
/  }
*/

   if (NOT unit_view (n)) {
      bug ("GET_NEIGHBOR: can't open:", n);  FAIL; }

   hinfo = hostptr->info;

   unit_read (n, line, L(0));                  /*   Caucus Host Name */
   chxcpy (hostptr->chn, line);
   unit_read (n, line, L(0));                  /*    Local Host Name */
   chxcpy (hostptr->lhname, line);
   hostptr->lhnumber = lhnum;                /*  Local Host Number */
   unit_read (n, line, L(0));                  /* remote CaucusMgrID */
   chxcpy (hinfo->mgrid, line);
   
   unit_read (n, line, L(0));                     /* Shipping Method */

   /*** We check this one just to make sure all is well (&it's easy!) */
   s = line;
   pos = 0;
   hinfo->shipmeth     = (int) chxint4 (s, &pos);
   hinfo->tosstime     = chxint4 (s, &pos);
   hinfo->reshiptime   = chxint4 (s, &pos);
   hinfo->shipmeth_urg = (int) chxint4 (s, &pos);
   if (hinfo->shipmeth == 0) {
      bug ("GET_NEIGHBOR: can't translate shipping method number.", 0);
      unit_close (n);  FAIL;
   }
   if (hinfo->tosstime   == 0)  hinfo->tosstime   = 14 * 24 * L(3600);
   if (hinfo->reshiptime == 0)  hinfo->reshiptime =      24 * L(3600);

   unit_read (n, line, L(0));                    /* How int4 to keep chunks */
   chxnum (line, &(hinfo->keep_chunks));
   if (hinfo->keep_chunks== 0)  hinfo->keep_chunks= 14 * 24 * L(3600);

   unit_read (n, line, L(0));                    /* Status of connection. */
   pos = 0;
   hinfo->status = (short) chxint4 (line, &pos);

   unit_read (n, line, L(0));
   pos = 0;
   hinfo->seqnum_to_low   = chxint4 (line, &pos);    /* XportFileSeq#ToLo */
   hinfo->seqnum_to_shipd = chxint4 (line, &pos);    /* XportFileSeq#ToShipd */
   hinfo->seqnum_to_hi    = chxint4 (line, &pos);    /* XportFileSeq#ToHi */

   /*** Let's check this line too, just to make sure it's all there */
   unit_read (n, line, L(0));                     /* XportFileSeq#From */
   pos=0;
   hinfo->seqnum_from = chxint4 (line, &pos);
   hinfo->ackseq      = chxint4 (line, &pos);
   if (pos < 3) {
      bug ("GET_NEIGHBOR: can't translate SeqNumFrom.", 0);
      unit_close (n);  FAIL;
   }

   unit_read (n, line, L(0));
   pos = 0;
   hinfo->seqnum_thru_low = chxint4 (line, &pos);
   hinfo->seqnum_thru_hi  = chxint4 (line, &pos);
  
   unit_close (n);

   SUCCEED;

/* if (n == XSNF) {                  See caching warning above.
      xsnf_first = 0;
      copyhost (myhost, hostptr);
   }
*/

done:
   chxfree (line);
   
   RETURN (success);
}   

/*
FUNCTION copyhost (destptr, srcptr)
HOST *destptr, *srcptr;
{
   chxcpy (destptr->chn,    srcptr->chn);
   chxcpy (destptr->lhname, srcptr->lhname);
   destptr->lhnumber = srcptr->lhnumber;
   *(destptr->info) = *(srcptr->info);
}
*/
