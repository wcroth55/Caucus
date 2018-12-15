/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** SET_NEIGHBOR.   Set all the information about a neighbor in
/                    XHI1 or XHI2.
/
/   int n = set_neighbor (hostptr, hunit)
/
/   Parameters:
/      HOSTPTR hostptr;
/      int     hunit;
/
/   Purpose:
/      This is the inverse of get_neighbor().  It stores this information
/      about a neighboring host:
/      1) CHN        2) LHName      3) LHNumber
/      4) Caucus ID of Caucus Mgr of Neighbor
/      5) Shipping method to use to send files
/      6) Status of Connection (Attached, Attached Pending, etc.)
/      7) Various sequence numbers used to coordinate shipping/receiving
/      from a struct of type HOST.
/
/      Set_neighbor() can also set the analogous values for the host,
/      which are stored in XSNF.  Set_neighbor detects this by checking
/      the value of hostptr->lhnum.  If it is zero, use XSNF.  Otherwise
/      use XHI1 or XHI2 and specify which it is in HUNIT.
/
/   How it works:
/      Set_neighbor writes the HOST structure out to XHI1, XHI2, or XSNF
/      using unit_write.  It assumes that the unit (XSNF or HUNIT) was locked
/      before the call to set_neighbor and that it will be unlocked
/      after returning.
/
/      To assist get_neighbor() with its (alleged) object-orientation,
/      set_neighbor flips XSNF_FIRST back to 1 (one) if it writes out
/      host info for *this* host, since get_neighbor caches this info.
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/
/   Error Conditions:
/
/   Related Functions: get_neighbor()
/
/   Called by: crexport(), 
/
/   Home: xaction/setneigh.c
/ */

/*: JV  5/02/91 11:23 Create this. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/05/91 10:18 Add entryex.h */
/*: JV  7/17/91 11:47 Minor mods. */
/*: JV  7/22/91 16:37 Add ENTRY */
/*: JV  7/26/91 13:26 Add code for XSNF. */
/*: JV  7/29/91 08:30 Add xsnf_first. */
/*: JV  8/09/91 12:19 Add HUNIT arg. */
/*: CR  8/08/91 21:59 Handle new tosstime,reshiptime members. */
/*: CR  8/13/91 15:44 Use %ld in sprintf's. */
/*: CR  8/20/91 21:30 Add ACKSEQ, keep_chunks members. */
/*: JV  8/26/91 16:33 Move keep_chunks to the right place. */
/*: DE  6/05/92 13:33 Chixified */

#include <stdio.h>
#include "handicap.h"
#include "entryex.h"
#include "unitcode.h"
#include "xfp.h"

#define PUTNEWLINE  unit_write (unit, chxquick("\n",0))

extern int xsnf_first;
extern union null_t null;

FUNCTION set_neighbor (hostptr, hunit)
   int     hunit;
   HOSTPTR hostptr;
{
   Chix line;
   int  unit;

   ENTRY ("set_neighbor", "");


   if (hostptr->lhnumber == 0) {unit=XSNF; xsnf_first=1;}
   else                         unit=hunit;

   if (NOT unit_make (unit)) {
      bug ("SET_NEIGHBOR: can't make: ", unit);  RETURN (0); }

   unit_write (unit, hostptr->chn);    PUTNEWLINE;      /* Caucus Host Name */
   unit_write (unit, hostptr->lhname); PUTNEWLINE;      /* Local Host Name */
   unit_write (unit, hostptr->info->mgrid); PUTNEWLINE; /* remote CaucusMgrID */

   line    = chxalloc (L(100), THIN, "setneigh line");

   /*** Shipping method and associated parameters. */
   chxformat (line, CQ("%d %d "), L(hostptr->info->shipmeth),
               hostptr->info->tosstime, null.chx, null.chx);
   chxformat (line, CQ("%d %d\n"), hostptr->info->reshiptime,
             L(hostptr->info->shipmeth_urg), null.chx, null.chx);
   unit_write (unit, line);

   /*** Receiving method stuff. */
   chxclear (line);
   chxformat (line, CQ("%d\n"), hostptr->info->keep_chunks, L(0), null.chx, 
              null.chx);
   unit_write (unit, line);

   chxclear  (line);
   chxformat (line, CQ("%d\n"), L(hostptr->info->status), L(0), null.chx, 
              null.chx);
   unit_write (unit, line);                  /* Status of connection. */

   chxclear  (line);
   chxformat (line, CQ("%d %d "), hostptr->info->seqnum_to_low,
            hostptr->info->seqnum_to_shipd, null.chx, null.chx);
   chxformat (line, CQ("%d\n"), hostptr->info->seqnum_to_hi, L(0), null.chx,
              null.chx);
   unit_write (unit, line);
   chxclear (line);
   chxformat (line, CQ("%d %d\n"), hostptr->info->seqnum_from,
                  hostptr->info->ackseq, null.chx, null.chx);
   unit_write (unit, line);
   chxclear (line);
   chxformat (line, CQ("%d %d\n"), hostptr->info->seqnum_thru_low,
            hostptr->info->seqnum_thru_hi, null.chx, null.chx);
   unit_write (unit, line);

   unit_close (unit);

   chxfree ( line );

   RETURN (1);
}   
