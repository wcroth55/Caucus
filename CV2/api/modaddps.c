/*** MOD_ADD_PSTAT.   Modify a Pstat when adding new conference text.
/
/    changed = mod_add_pstat (pstat, inum, rnum, mytext);
/
/    Parameters:
/       int     changed;  (was pstat changed?)
/       Pstat   pstat;    (user's pstat record)
/       int     inum;     (number of item with new material)
/       int     rnum;     (number of new response; 0 => new item)
/       int     mytext;   (when my text is new: 0=>later, 1=>now, 2=>never)
/
/    Purpose:
/       When new material is added to a conference, the user's Pstat
/       (record of material "seen") may need to be modified.  
/       Mod_add_pstat() encapsulates all the rules for deciding when
/       (and what parts of) the Pstat must be modified.
/
/    How it works:
/       Each time new material is added to an item, the UI should call
/       mod_add_pstat().  When adding a new response, specify the
/       item and response number; when adding a new item, specify the
/       item number, and use 0 for the response number.
/
/       For more information about MYTEXT, see the service function
/       get_setting().
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
/    Home:  api/modaddps.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/05/93 12:39 New function. */
/*: CR 10/22/93 14:54 Modify rules for new resp & SET MYTEXT never. */
/*: CR  4/10/95 15:41 Fix MYTEXT LATER bug. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  mod_add_pstat (Pstat pstat, int inum, int rnum, int mytext)
{
   int    i, changed;

   changed = 0;

   /*** New item, MYTEXT NEVER: mark INUM as seen, inbetween items UNSEEN. */
   if (rnum==0  &&  mytext==2) {
      for (i=pstat->items+1;   i<inum;   ++i) {
         pstat->resps[i] = -1;   /* UNSEEN. */
         pstat->back [i] =  0;
      }
      pstat->resps[inum] =  0;
      pstat->back [inum] =  0;
      pstat->items       = inum;
      changed = 1;
   }

   /*** New response, MYTEXT NEVER: make all resps on this item SEEN. */
   if (rnum > 0  &&  mytext==2) {
      if (pstat->resps[inum] == rnum-1) {
         pstat->resps[inum] = rnum;
         changed = 1;
      }
      if (pstat->back[inum] != 0)  changed = 1;
      pstat->back [inum] = 0;
   }

   /*** New response, MYTEXT LATER, only "new" responses are by this user:
   /    adjust resps/back so those responses will really become new only
   /    when someone *else* adds a response. */
   if (rnum > 0  &&  mytext==0  &&  pstat->resps[inum] >= rnum-1) {
      pstat->resps[inum] = rnum;
      pstat->back [inum]++;
      changed = 1;
   }

   return (changed);
}
