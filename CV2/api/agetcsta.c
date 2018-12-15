/*** A_GET_CSTAT.   Get status (# items/resps) information about a conference.
/
/    ok = a_get_cstat (cstat, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Cstat     cstat;    (object to fill with status info)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Load status information (#items, #responses on each item) about
/       a conference into the CSTAT object.
/
/    How it works:
/       Caller must fill in cstat->cnum.  cstat->items becomes number of
/       highest item in conference.  cstat->resps[i] becomes number of
/       responses for item I.  0 means item with no responses.  -1 means 
/       item was deleted.
/
/       Note that there is no a_chg_cstat().  Changes to number of items
/       or responses occur automatically (to the disk database) when items
/       or responses are added by a_add_resp().
/
/    Returns: A_OK on success
/             A_NOCONF if no such conference
/             A_DBERR on database error
/
/    Error Conditions:
/
/    Side effects: 
/      (a) For now, we use the global "master".  We may eventually want
/          to replace this with a master struct allocated & freed locally.
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/agetcsta.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/30/92 22:47 New function */
/*: CR 12/01/92 12:06 Use <= master.items. */
/*: CR 12/01/92 14:00 Remove unused globals. */
/*: CR 12/13/92 22:06 Add comments. */
/*: CR  8/13/98 22:08 Add 'time' (age of data) element. */

#include <stdio.h>
#include <time.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

extern struct master_template master;
extern int                    debug;

FUNCTION  a_get_cstat (Cstat cstat, int4 mask, int wait)
{
   int    success, i;

   if (cstat == (Cstat) NULL  ||  cstat->tag != T_CSTAT)  return (A_BADARG);
   if (cstat->cnum <= 0)                                  return (A_NOCONF);

   if (NOT srvldmas (1, &master, cstat->cnum))  DONE (A_NOCONF);

   chxcpy (cstat->owner, master.organizerid);
   cstat->items = master.items;
   for (i=0;   i<=master.items;   ++i)  cstat->resps[i] = master.responses[i];

   cstat->time  = time(NULL);
   cstat->ready = 1;
   success = A_OK;

done:
   return (success);
}
