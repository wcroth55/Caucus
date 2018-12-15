/*** MOD_MARK_PSTAT.   Modify a Pstat when marking conference text.
/
/    changed = mod_mark_pstat (pstat, inum, rnum);
/
/    Parameters:
/       int     changed;  (was pstat changed?)
/       Pstat   pstat;    (user's pstat record)
/       int     inum;     (number of item viewed)
/       int     rnum;     (highest response seen)
/
/    Purpose:
/       When a user marks a particular response as "seen", the
/       user's Pstat (record of material "seen") may need to be modified.  
/       Mod_mark_pstat() encapsulates all the rules for deciding when
/       (and what parts of) the Pstat must be modified.
/
/    How it works:
/       Each time that a response is specifically marked as "highest
/       seen", the UI should call mod_mark_pstat().
/
/       Note: this function is derived from mod_view_pstat(), for
/       the express purpose of use in WebCaucus.
/
/    Returns: 1 if pstat was changed at all, 0 otherwise.
/       The caller can use this information to determine if and
/       when the changed pstat should be written back to the database
/       with a_chg_pstat().  Mod_add_pstat() does NOT write to the database!
/
/    Error Conditions:
/  
/    Called by: UI
/
/    Known bugs:      none
/
/    Home:  modmarkp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/10/95 15:07 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "entryex.h"
#include "api.h"

FUNCTION  mod_mark_pstat (pstat, inum, rnum)
   Pstat  pstat;
   int    inum, rnum;
{
   int    i, changed;
   ENTRY ("mod_mark_pstat", "");

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

   /*** If the last response "marked seen" hasn't moved, no change. */
   if (rnum == pstat->resps[inum] - pstat->back[inum])  RETURN (changed);

   /*** Otherwise, set to that response. */
   pstat->resps[inum] = rnum;
   pstat->back [inum] = 0;

   RETURN (1);
}
