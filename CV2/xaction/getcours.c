/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** GET_COURSE.   Figure out which neighbor to send thru to get to a node.
/
/   int n = get_course (chn, lhnum)
/
/   Parameters:
/      char *chn;      (CHN of the node)
/      int  *lhnum;    (LHN of the neighbor enroute to CHN)
/
/   Purpose:
/      Both remote e-mail and conference change xactions may be headed
/      for non-neighboring nodes.  Our "Imlict Dynamic Routing" method
/      specifies that each of these xactions is assigned a unique 
/      destination, but that none of the nodes aint4 the way knows
/      the whole path - just about which *direction* to send the xaction
/      to get it to the destination.  Get_course() figures out what
/      this direction is, in the form of "who is the next node".
/      Get_course returns this information in the form of the LH#
/      of the neighbor.  It returns it via LHNUM.
/
/   How it works:
/      Get_course() checks the routing table.
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
/   Home: xaction/getcours.c
/ */

/*: JV  6/12/91 10:20 Create this. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/30/91 08:45 Add ENTRY/EXIT stuff. */
/*: JV  8/02/91 08:42 Actually write the code! */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: CR  8/13/91 11:54 Declare strtoken(). */
/*: JV 10/08/91 12:00 Use XSRD instead of XSCD for routing. */
/*: JV 11/06/91 10:29 Change common.h to null.h, add namelist.h. */
/*: CR 11/07/91 14:19 Add confnum arg to matchnames() call. */
/*: JV 12/03/91 12:28 Allow CHN substrings, remove npfbuf.. */
/*: JV  1/03/92 15:18 Declare strcpy(). */
/*: DE  6/03/92 14:32 Chixified  */

#include <stdio.h>
#include "handicap.h"
#include "unitcode.h"
#include "textbuf.h"
#include "xaction.h"
#include "null.h"
#include "namelist.h"
#include "done.h"

extern union null_t null;

FUNCTION  get_course (chn, lhnum)
   int  *lhnum;
   Chix  chn;
{
   struct namlist_t *list, *owners, *this, *maybe, *nlnode();
   Chix   hname, snum;
   int4   hnum=0;
   int    ok, size, success;
   char  *strtoken(), *strcpy();

   ENTRY ("get_course", "");

   hname    = chxalloc (L(80), THIN, "getcours hname");
   snum     = chxalloc (L(8),  THIN, "getcours snum");
   success  = 0;

   /*** Get namelist of hosts matching CHN. */
   list = nlnode (1);
   nladd (list, chn, 0);
   ok = matchnames (XSRD, XSRN, 0, null.chx, list, AND, &owners, NOPUNCT);
   nlfree (list);
   if (ok != 1) {
      bug ("GET_COURSE: can't get routing table.", 0); FAIL; }

   /*** If no matches, complain and return. */
   if ( (size = nlsize (owners)) == 0) {
      bug ("GET_COURSE: looking for host not in routing table.", 0);
      nlfree (owners);
      FAIL;
   }

   /*** Scan the list of matches and find the one exact match. */
   maybe = null.nl;
   for (this=owners->next;   this!=null.nl;   this = this->next) {
      chxtoken (hname, null.chx, 1, this->str);
      if (NOT chxeq (chn, hname)) { maybe = this; continue; }
      chxtoken (snum, null.chx, 2, this->str);
      chxnum   (snum, &hnum);
   }
   nlfree (owners);
 
   /*** Use initial substring match? */
   if (hnum == 0 && size == 1 && maybe != null.nl) {
      chxtoken (hname, null.chx, 1, maybe->str);
      chxtoken (snum,  null.chx, 2, maybe->str);
      chxnum   (snum, &hnum);
   }
   *lhnum = hnum;

   if (hnum) { chxcpy (chn, hname); SUCCEED; }
   else {
      bug ("GET_COURSE: couldn't find host in routing table.", 0);
      FAIL;
   }
 done:
   chxfree ( hname );
   chxfree ( snum );

   RETURN ( success );
}   
