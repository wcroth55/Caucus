/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** R_BOUND.   Apply boundary checking to response range numbers.
/
/    ok = r_bound (first, last, final, rl, cnum, inum, maxresp);
/
/    Parameters:
/       int    ok;          (Valid response range produced?)
/       int   *first;       (first response number in range)
/       int   *last;        (last undeleted response number in range)
/       int   *final;       (last  response number in range)
/       Rlist  rl;          (node of Rlist defining initial response range)
/       int    cnum;        (conference number)
/       int    inum;        (item number)
/       int    maxresp;     (maximum number of responses for this item)
/
/    Purpose:
/       Apply boundary checking to response range numbers.
/
/       Any time that an Rlist is used to actually refer to a specific
/       response range on a particular item, r_bound() should be called
/       to apply boundary checking to that response range.
/
/    How it works:
/       R_bound() checks for occurences such as:
/           response number "-1" used to refer to last response in item
/           first response in range > last response in range
/
/        and translates the values in RL to actual "hard" response
/        numbers in FIRST, LAST, and FINAL.  (FINAL and LAST will only
/        be different if there are deleted responses at the very end
/        of an item.  FINAL will include the deleted responses, 
/        LAST will not.)
/
/    Returns: 1 if returned FIRST/LAST/FINAL refer to a real response range
/             0 if they imply an impossible response range
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  gen/rbound.c
/ */

/*: CP  8/06/93 11:56 New function. */
/*: CP  8/10/93 13:22 Do not upper bound FIRST. */
/*: CP 11/18/93 15:23 Add cnum/inum args; LAST -> last non deleted resp. */
/*: CP 12/10/93 21:29 Add 'final' argument. */

#include <stdio.h>
#include "caucus.h"
#include "api.h"

FUNCTION  r_bound (first, last, final, rl, cnum, inum, maxresp)
   int   *first, *last, *final;
   Rlist  rl;
   int    cnum, inum, maxresp;
{
   Resp   robj;
   int    ok;
   int    lastund;     /* Last non-deleted response. */

   ENTRY ("r_bound", "");

  *final = rl->r1;
  *last  = rl->r1;
  *first = rl->r0;

   /*** If the Rlist entry contained a reference to response -1 (aka "LAST"),
   /    scan the responses backwards from the end to find the FINAL non-
   /    deleted response. */
   if (*last < 0  ||  *first < 0) {
      robj = a_mak_resp ("r_bound robj");
      robj->cnum = cnum;
      robj->inum = inum;
      for (lastund=maxresp;   lastund>0;   --lastund) {
         robj->rnum = lastund;
         ok = a_get_resp (robj, P_TEXT, A_WAIT);
         if (ok == A_OK  &&  NOT EMPTYCHX(robj->owner))  break;
      }
      a_fre_resp (robj);
   }

   if (*final <  0)       *final = maxresp;
   if (*last  <  0)       *last  = lastund;
   if (*last  > maxresp)  *last  = maxresp;
   if (*first <  0)       *first = lastund;

   RETURN (*first <= *final);
}
