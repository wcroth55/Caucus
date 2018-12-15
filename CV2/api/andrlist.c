
/*** AND_RLIST.   Take the logical "and" of two Rlist's.
/
/    and = and_rlist (rla, rlb);
/
/    Parameters:
/       Rlist     and;      (returned "and" result)
/       Rlist     rla;      (1st Rlist)
/       Rlist     rlb;      (2nd Rlist)
/
/    Purpose:
/       Produce a new Rlist which is the logical "and" of two
/       existing Rlist's.
/
/    How it works:
/       And_rlist() just compares each node in RLA against every 
/       node in RLB.  The result is placed in a new
/       Rlist which and_rlist() allocates and returns.  (It is up
/       to the caller to eventually free the new Rlist with
/       a_fre_rlist().)
/
/       Note that and_rlist() can "succeed" by returning a completely
/       empty Rlist (meaning RLA and RLB have nothing in common).
/
/    Note:
/       And_rlist() depends on the fact that no actual response number
/       can be higher than HIGHBOUND.
/
/    Returns: new Rlist
/             NULL on error
/
/    Error Conditions:
/       RLA or RLB not an Rlist.
/
/    Side effects: 
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/andrlist.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  5/17/93 21:31 New function. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

#define  RLNULL     ((Rlist) NULL)
#define  HIGHBOUND  20000

FUNCTION  Rlist and_rlist (Rlist rla, Rlist rlb)
{
   int    ilo, ihi, rlo, rhi;
   int    pa_r0, pa_r1, pb_r0, pb_r1;
   Rlist  result, tail, pa, pb;

   /*** Check arguments. */
   if (rla==RLNULL           ||  rlb==RLNULL)           return (RLNULL);

   /*** RESULT is the new Rlist we are building.  TAIL points to the
   /    last node in the Rlist.  New nodes are always added at the
   /    tail. */
   result = a_mak_rlist("andrlist result");
   tail   = result;

   /*** For each node in RLA, compare it against every node in RLB. */
   for (pa=rla->next;   pa!=RLNULL;   pa=pa->next) {
      for (pb=rlb->next;   pb!=RLNULL;   pb=pb->next) {

         /*** Compute the bounding rectangle.  If it vanishes, there
         /    is no intersection between these two nodes. */
         ilo = max (pa->i0, pb->i0);
         ihi = min (pa->i1, pb->i1);
         if (ihi < ilo)  continue;

         pa_r0 = pa->r0;   if (pa_r0 == -1)  pa_r0 = HIGHBOUND;
         pa_r1 = pa->r1;   if (pa_r1 == -1)  pa_r1 = HIGHBOUND;
         pb_r0 = pb->r0;   if (pb_r0 == -1)  pb_r0 = HIGHBOUND;
         pb_r1 = pb->r1;   if (pb_r1 == -1)  pb_r1 = HIGHBOUND;
         rlo = max (pa_r0, pb_r0);
         rhi = min (pa_r1, pb_r1);
         if (rhi < rlo)  continue;

         if (rlo == HIGHBOUND)  rlo = -1;
         if (rhi == HIGHBOUND)  rhi = -1;
         tail->next = a_mak_rlist("andrlist next");
         tail     = tail->next;
         tail->i0 = ilo;   tail->i1 = ihi;
         tail->r0 = rlo;   tail->r1 = rhi;
      }
   }

   return (result);
}
