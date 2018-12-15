/*** MOD_VIEW_PSTAT.   Modify a Pstat when viewing conference text.
/
/    changed = mod_view_pstat (pstat, inum, rnum);
/
/    Parameters:
/       int     changed;  (was pstat changed?)
/       Pstat   pstat;    (user's pstat record)
/       int     inum;     (number of item viewed)
/       int     rnum;     (highest response seen)
/
/    Purpose:
/       When a user views (part of) an item, the user's Pstat
/       (record of material "seen") may need to be modified.  
/       Mod_view_pstat() encapsulates all the rules for deciding when
/       (and what parts of) the Pstat must be modified.
/
/    How it works:
/       Each time that (part of) an item is viewed, the UI should call
/       mod_view_pstat().
/
/    Returns: 1 if pstat was changed at all, 0 otherwise.
/       The caller can use this information to determine if and
/       when the changed pstat should be written back to the database
/       with a_chg_pstat().  Mod_add_pstat() does NOT write to the database!
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by: UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/modviewp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/07/93 13:47 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  mod_view_pstat (Pstat pstat, int inum, int rnum)
{
   int    i, changed;

   changed = 0;

   /*** Viewing a new item... */
   if (inum > pstat->items) {

      /*** Mark all "new" items (up to and including this one) as UNSEEN. */
      for (i=pstat->items+1;   i<=inum;   ++i) {
         pstat->resps[i] = -1;
         pstat->back [i] =  0;
      }
      pstat->items = inum;
      changed = 1;
   }

   
   /*** If we've seen these responses already, just go home. */
   if (rnum < pstat->resps[inum] - pstat->back [inum])  return (changed);

   /*** Brand-new responses.  Mark them seen. */
   if (rnum >= pstat->resps[inum]) {
      pstat->resps[inum] = rnum;
      pstat->back [inum] = 0;
      return (1);
   }

   /*** Only remaining case falls into MYTEXT LATER category:
   /    responses this user entered but had not "seen". */
   pstat->back[inum] = pstat->resps[inum] - rnum - 1;
   return (1);
}
