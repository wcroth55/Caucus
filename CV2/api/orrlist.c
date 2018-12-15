
/*** OR_RLIST.   Take the logical "or" of two Rlist's.
/
/    or = or_rlist (rla, rlb);
/
/    Parameters:
/       Rlist     or;       (returned "or" result)
/       Rlist     rla;      (1st Rlist)
/       Rlist     rlb;      (2nd Rlist)
/
/    Purpose:
/       Produce a new Rlist which is the logical "or" of two
/       existing Rlist's.
/
/    How it works:
/       or_rlist() "walks" down the items that appear in RLA and RLB, and 
/       adds them to a new list, merging where overlaps occur.
/       The result is placed in a new Rlist which or_rlist() allocates 
/       and returns.  (It is up to the caller to eventually free the 
/       new Rlist with a_fre_rlist().)
/
/    Note:
/       or_rlist() depends on the fact that no actual response number
/       can be higher than HIGHBOUND.
/
/    Returns: new Rlist
/             NULL on error
/
/    Error Conditions:
/       RLA or RLB not an Rlist.
/
/    Called by:  UI
/
/    Known bugs:
/
/    Home:  api/orrlist.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/06/96 17:54 New function */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

#define  RLNULL            ((Rlist) NULL)
#define  HIGHBOUND         20000
#define  INSIDE(item, px)  (px!=RLNULL  &&  item>=px->i0  &&  item<=px->i1)

FUNCTION  Rlist or_rlist (Rlist rla, Rlist rlb)
{
   int    ilo, ihi, rlo, rhi;
   int    pa_r0, pa_r1, pb_r0, pb_r1, item;
   Rlist  result, tail, pa, pb, pr;

   /*** Check arguments. */
   if (rla==RLNULL  ||  rlb==RLNULL)       return (RLNULL);

   /*** RESULT is the new Rlist we are building.  TAIL points to the
   /    last node in the Rlist.  New nodes are always added at the
   /    tail. */
   result = a_mak_rlist("orrlist result");
   tail   = result;
   pa = rla->next;
   pb = rlb->next;

   ilo = HIGHBOUND;
   if (pa != RLNULL)  ilo = min (ilo, pa->i0);
   if (pb != RLNULL)  ilo = min (ilo, pb->i0);

   /*** Consider all item #s, starting at the lowest of RLA or RLB, and
   /    going up to the highest of RLA or RLB. */
   for (item=ilo;   (1);   ++item) {

      if (pa == RLNULL  &&  pb == RLNULL)   break;

      if (pa != RLNULL  &&  item > pa->i1)  pa = pa->next;
      if (pb != RLNULL  &&  item > pb->i1)  pb = pb->next;


      /*** Find the lowest and highest Response value in the overlap. */
      rlo = HIGHBOUND;
      rhi = 0;
      if (INSIDE (item, pa)) {
         pa_r0 = pa->r0;   if (pa_r0 == -1)  pa_r0 = HIGHBOUND;
         pa_r1 = pa->r1;   if (pa_r1 == -1)  pa_r1 = HIGHBOUND;
         rlo = min (rlo, pa_r0);
         rhi = max (rhi, pa_r1);
      }
      if (INSIDE (item, pb)) {
         pb_r0 = pb->r0;   if (pb_r0 == -1)  pb_r0 = HIGHBOUND;
         pb_r1 = pb->r1;   if (pb_r1 == -1)  pb_r1 = HIGHBOUND;
         rlo = min (rlo, pb_r0);
         rhi = max (rhi, pb_r1);
      }

      if (rlo > rhi)  continue;

      if (item == ilo) { 
         tail->next = a_mak_rlist("orrlist tail");
         tail     = tail->next;
         tail->i0 = tail->i1 = item;
         tail->r0 = rlo;
         tail->r1 = rhi;
      }

      else if (tail->i1+1 == item  &&  tail->r0 == rlo  &&  tail->r1 == rhi) {
         tail->i1 = item;
      }

      else {   /* Start a new RL node. */
         tail->next = a_mak_rlist("andrlist next");
         tail     = tail->next;
         tail->i0 = tail->i1 = item;
         tail->r0 = rlo;
         tail->r1 = rhi;
      }
   }


   /*** Fixup any high-response numbers that got changed to HIGHBOUND. */
   for (pr=result->next;   pr!=RLNULL;   pr=pr->next) {
      if (pr->r1 == HIGHBOUND)  pr->r1 = -1;
   }
      
   return (result);
}
