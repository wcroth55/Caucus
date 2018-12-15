/*** A_CHG_PSTAT.   Rewrite status (# items/resps read ) info about a person.
/
/    ok = a_chg_pstat (pstat, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Pstat     pstat;    (object containing status info)
/       int4      mask;     (mask of properties to write)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Write status information (#items, #responses read on each item) about
/       a person from the PSTAT object.
/
/    How it works:
/       Caller must fill in pstat->cnum, pstat->owner, pstat->items,
/       pstat->resps, and pstat->back.  See a_get_pstat() for information
/       amount the meaning of the contents.
/
/    Returns: A_OK on success
/             A_NOCONF    if no such conference
/             A_NOREAD    person not allowed in conference
/             A_NOPERSON  if no such person
/             A_NOTOWNER  owner is not current userid
/             A_NOTMEMBR  not a member
/
/    Error Conditions:
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
/       This version uses the global "thisconf", for efficiency(?).
/       Future versions may wish to allocate their own local
/       partic_t.
/
/    Home:  api/agetpsta.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/09/92 21:34 New function. */
/*: CR 12/13/92 22:31 Add comments. */
/*: CR  2/22/93 23:31 Add new args to a_cache() calls. */
/*: CR  8/20/04 Modify to handle XUPA (user part.) or XCUP (hidden items). */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "done.h"

extern struct partic_t  thisconf;

FUNCTION  a_chg_pstat (Pstat pstat, int4 mask, int wait)
{
   int    success, i, access, additem, chgresp, ok, chgsubj, error, unit;
   Chix   userid;

   if (disk_failure (0, &error, NULL))                    return (A_DBERR);

   /*** Check validity of arguments. */
   if (pstat == (Pstat) NULL  ||  pstat->tag != T_PSTAT)  return (A_BADARG);
   if (pstat->cnum <= 0)                                  return (A_NOCONF);
   unit = (EMPTYCHX(pstat->owner) ? XCUP : XUPA);

   /*** User may only affect their own pstat. */
   userid = chxalloc (L(20), THIN, "a_chg_ps userid");
   sysuserid (userid);
   if (unit==XUPA  &&  NOT chxeq (userid, pstat->owner))  DONE   (A_NOTOWNER);

   /*** Make sure user can access this conference. */
   ok = a_cache (V_GET, pstat->cnum, &access, &additem, &chgresp,
                        &chgsubj, nullchix);
   if (access < 1)                                        DONE   (A_NOREAD);

   /*** Make sure user is already a member of this conference. */
   if (NOT unit_lock  (unit, WRITE, L(pstat->cnum), L(0), L(0), L(0), userid))
                                                          DONE   (A_NOTMEMBR);
   if (NOT unit_check (unit)) { unit_unlk (unit);         DONE   (A_NOTMEMBR); }

   thisconf.items = pstat->items;
   for (i=0;   i<=thisconf.items;   ++i) {
      thisconf.responses[i] = pstat->resps[i];
      thisconf.backresp [i] = pstat->back [i];
   }

   ok = store_user (&thisconf, unit);
   unit_unlk (unit);
   if (NOT ok)    DONE  (A_NOPERSON);

   pstat->ready = 1;
   DONE (A_OK);

done:
   chxfree (userid);
   return  (success);
}
