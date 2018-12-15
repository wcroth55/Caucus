
/*** ONE_RESPONSE.   Get first "real" response from an Rlist.
/
/    one_response (&item, &resp, rl, master);
/
/    Parameters:
/       int      *item;      (place to return item     number)
/       int      *resp;      (place to return response number)
/       Rlist     rl;        (node of Rlist containing desired response)
/       Master    master;    (master struct for this conf)
/
/    Purpose:
/       Sometimes we only want the very first response in an Rlist.
/       One_response() is handed an Rlist, and finds the first 
/       "real" response.
/
/    How it works:
/       Scans Rlist RL in order until it finds a response that could
/       really exist.  (Skips items with impossible item numbers, 
/       translates response numbers of -1 to actual last response, etc.)
/       Sets RESP and ITEM appropriately.
/
/       If no response is found, sets ITEM and RESP to -1.
/
/    Returns: 1 on success
/             0 if no response is found.
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/oneresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  8/10/93 13:16 New function. */
/*: CP 11/18/93 15:35 Add cnum, inum args to r_bound() call. */
/*: CP 12/10/93 21:31 Add 'final' arg to r_bound() call. */

#include <stdio.h>
#include "caucus.h"
#include "api.h"
#define   RLNULL   ( (Rlist) NULL )

FUNCTION  one_response (int *item, int *resp, Rlist rl, Master master)
{
   Rlist     rptr;
   int       last, final;

   ENTRY ("one_response", "");

   for (rptr=rl;   rptr!=RLNULL;   rptr=rptr->next) {
      for (*item=rptr->i0;   *item<=rptr->i1;   ++*item) {
         if (*item<=0)      continue;
         if (NOT r_bound (resp, &last, &final, rptr, master->cnum, *item,
                          master->responses[*item]))   continue;
         RETURN (1);
      }
  }

  *item = *resp = -1;
  RETURN (0);
}
