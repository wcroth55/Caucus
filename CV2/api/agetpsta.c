/*** A_GET_PSTAT.   Get status (# items/resps read ) info about a person.
/
/    ok = a_get_pstat (pstat, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Pstat     pstat;    (object to fill with status info)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Load status information (#items, #responses read on each item) about
/       a person into the PSTAT object.
/
/    How it works:
/       Caller must fill in pstat->cnum, pstat->owner.  Function fills in:
/         pstat->items    (highest numbered item seen by 'owner')
/         pstat->resps[i] (# resps seen on item I; -1=> unseen; -2=>forgotten)
/         pstat->back[i]  (# resps on item I to "go back" from pstat->resps[i]
/         pstat->lastin   (day/time last in this conf)
/            during display -- see Caucus SET MYTEXT LATER command).
/
/       If pstat->owner is empty or null, then this is a conference
/       "hidden items" record, which is formatted like a pstat.
/
/    Returns: A_OK on success
/             A_NOCONF   if no such conference
/             A_NOPERSON if no such person
/             A_NOTMEMBR not a member
/             A_DBERR    on database error
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
/       partic_t, or use a "hidden" cached copy.
/
/    Home:  api/agetpsta.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/01/92 13:56 New function. */
/*: CR 12/13/92 22:28 Add comments. */
/*: CR  8/20/04 Add XUPA unit argument to load_user() call. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

extern struct partic_t  thisconf;

FUNCTION  a_get_pstat (Pstat pstat, int4 mask, int wait)
{
   int    success, i, is_member, unit;

   if (pstat == (Pstat) NULL  ||  pstat->tag != T_PSTAT)  return (A_BADARG);
   if (pstat->cnum <= 0)                                  return (A_NOCONF);

   unit = (EMPTYCHX (pstat->owner) ? XCUP : XUPA);

   if (NOT unit_lock (unit, READ, L(pstat->cnum), L(0), L(0), L(0), 
                      pstat->owner))   DONE (A_NOPERSON);
   is_member = load_user (&thisconf, unit);
   unit_unlk (unit);

   chxclear (pstat->lastin);
   if (NOT is_member)  DONE (A_NOTMEMBR);

   chxcpy (pstat->lastin, thisconf.lastin);
   pstat->items = thisconf.items;
   for (i=0;   i<=thisconf.items;   ++i) {
      pstat->resps[i] = thisconf.responses[i];
      pstat->back [i] = thisconf.backresp [i];
   }

   pstat->ready = 1;
   DONE (A_OK);

done:
   return  (success);
}
