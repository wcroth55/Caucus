/*** AND_FROZEN: Reduce an Rlist to only those items that are frozen.
/
/    ok = and_frozen (cnum, master, rlist);
/
/    Parameters:
/       int    ok;                       (Success?)
/       int    cnum;                     (local number of conference to be sent)
/       struct master_template *master;  (Pointer to a master template)
/       Rlist  rlist;                    (list to be "anded")
/
/    Purpose:
/       Reduce the contents of Rlist RL to only include those items
/       which are frozen.
/
/    How it works:
/       If MASTER does not contain the item info for conference CNUM,
/       loads MASTER from disk.
/
/       If and_frozen() fails, RLIST is not modified.
/
/       If no items are frozen, RLIST is emptied.
/
/    Returns: 1 on success (includes emptying RLIST if no frozen items)
/             0 on error (cannot use or reload MASTER)
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  cmd2/freeze.c, gen/newparse.c
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/andfroze.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  9/08/93 15:12 New function, derived from old get_frozen(). */

#include <stdio.h>
#include "caucus.h"
#include "master.h"
#include "api.h"
#define   RLNULL   ( (Rlist) NULL )

FUNCTION  and_frozen (cnum, master, rlist)
   int    cnum;
   struct master_template *master;
   Rlist  rlist;
{
   int    item, success;
   int4   frnum;
   Chix   fr_str, header, title;
   Rlist  result, fptr, rl, add_to_rlist();

   ENTRY ("and_frozen", "");

   title   = chxalloc (L(80), THIN, "andfroze title");
   fr_str  = chxalloc (L(80), THIN, "andfroze fr_str");
   header  = chxalloc (L(80), THIN, "andfroze header");
   result  = a_mak_rlist ( "andfroze frozen" );
   success = 0;

   if (master->cnum != cnum  &&  NOT srvldmas (1, master, cnum))   FAIL;

   sysbrkclear();
   for (fptr=result, rl=rlist;   rl!=RLNULL;  rl=rl->next) {
      for (item=rl->i0;   item<=rl->i1;   ++item) {
         if (item <= 0)                    continue;
         if (master->responses[item] < 0)  continue;   /* Deleted. */
         if (sysbrktest())                 break;

         load_header (cnum, item, header, title, (Attachment) NULL);
         chxtoken (fr_str, nullchix, 3, header);
         chxnum   (fr_str, &frnum);
         if (frnum)  fptr = add_to_rlist (fptr, item, rl->r0, rl->r1);
      }
   }

   /*** Rehook RESULT to RLIST, and free unused parts. */
   a_fre_rlist (rlist->next);
   rlist->next = result->next;
   result->next = RLNULL;
   SUCCEED;

 done:
   a_fre_rlist (result);
   chxfree (title);
   chxfree (header);
   RETURN  (success);
}
