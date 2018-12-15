/*** A_ADD_PSTAT.   Add a person to a conference as a member.
/
/    ok = a_add_pstat (pstat, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Pstat     pstat;    (object containing status info)
/       int4      mask;     (ignored)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Add a person to a conference as a member.  Initialize their
/       "status" record for this conference to "nothing seen".
/
/    How it works:
/       Caller must fill in pstat->cnum, pstat->owner.
/
/    Returns: A_OK on success
/             A_NOCONF    if no such conference
/             A_NOREAD    person not allowed in conference
/             A_NOPERSON  if no such person
/             A_NOTOWNER  owner is not current userid
/             A_NOTNEW    already a member
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

/*: CR 12/09/92 22:40 New function. */
/*: CR 12/13/92 22:29 Add comments. */
/*: CR  2/22/93 23:31 Add new args to a_cache() calls. */
/*: CP  5/06/92 12:30 Add L(0) as modnames 'item' arg. */
/*: CP  8/01/93  9:37 Add debugging stuff. */
/*: CR  5/15/95 21:15 Clear pstat's # of items seen. */
/*: CR  5/01/04 If override (userid != owner), get access info directly! */
/*: CR  8/20/04 Add 2nd arg to store_user() call. */
/*: CR 10/29/05 Remove adding person to membership list. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "done.h"

extern struct partic_t  thisconf;

FUNCTION  a_add_pstat (Pstat pstat, int4 mask, int wait)
{
   int    success, access, additem, chgresp, ok, chgsubj, error, unit;
   Chix   userid, str;
   Person person;
   Namelist add, del, groups, nlnode();

   if (disk_failure (0, &error, NULL))                    return (A_DBERR);

   /*** Check validity of arguments. */
   if (pstat == (Pstat) NULL  ||  pstat->tag != T_PSTAT)  return (A_BADARG);
   if (pstat->cnum <= 0)                                  return (A_NOCONF);
   unit = (EMPTYCHX(pstat->owner) ? XCUP : XUPA);

   /*** Allocate miscellaneous stuff. */
   person = a_mak_person ("aaddpsta person");
   userid = chxalloc (L(20), THIN, "a_add_ps userid");
   str    = chxalloc (L(40), THIN, "a_add_ps str");

   /*** User may only affect their own pstat. */
   sysuserid (userid);
   if (NOT (mask & P_OVERRIDE)) {
      if (NOT chxeq (userid, pstat->owner))               DONE   (A_NOTOWNER);
      ok = a_cache (V_GET, pstat->cnum, &access, &additem, &chgresp,
                           &chgsubj, nullchix);
   }
   else {
      groups = nlnode (1);
      access = allowed_on ( (unit == XUPA ? pstat->owner : userid),  
                             groups, pstat->cnum);
      nlfree (groups);
   }

   /*** Make sure user can access this conference. */
   if (access < 1)                                        DONE   (A_NOREAD);

   /*** Make sure user is not already a member of this conference. */
   if (NOT unit_lock  (unit, WRITE, L(pstat->cnum), L(0), L(0), L(0), 
           pstat->owner))                                 DONE   (A_DBERR);
   if (unit_check (unit))     { unit_unlk (unit);         DONE   (A_NOTNEW); }

   /*** Initialize user's record to "nothing seen". */
   thisconf.items = 0;
   pstat->items   = 0;
   ok = store_user (&thisconf, unit);
   unit_unlk (unit);
   if (NOT ok)       DONE (A_NOPERSON);

   pstat->ready = 1;
   DONE (A_OK);

done:
   a_fre_person (person);
   chxfree (userid);
   chxfree (str);
   return  (success);
}
