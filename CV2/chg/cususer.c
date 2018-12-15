
/*** CUS_USER.   Customize (edit) user list for this conference.
/
/    ok = cus_user();
/
/    Parameters:
/       int    ok;          (Success?)
/
/    Purpose:
/
/    How it works:
/
/    Returns: 1 on success
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  customize()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  chg/cususer.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/30/91 22:44 New function. */
/*: CR 11/05/91 18:08 CaucusLink version just distributes entire userlist. */
/*: UL 12/11/91 18:41 Make unit_lock args 3-6 int4.*/
/*: DE  3/16/92 17:10 Chixified - success->pass (For done.h)*/
/*: JX  5/19/92 20:36 Fix Chix. */
/*: JX  8/03/92 10:10 CHN's are chix. */
/*: JX  8/10/92 11:15 Flush allowed_on's cache. */
/*: CR 12/07/92 15:12 Add unit arg to store_ulist(). */
/*: CR 04/12/04 Use co_priv_str() for conf priv nums. */

#include <stdio.h>
#include "caucus.h"

extern Userreg           thisuser;
extern union  null_t     null;
extern short             confnum;

FUNCTION  cus_user()
{
   Chix      ul0;
   Chix      ctext, priorg;
   int       errors, pass, success;
   Namelist  groups;
   Namelist  nlnode();

   ENTRY ("cus_user", "");


   /*** Copy the XCUS file into a temporary, editable file. */
   if (NOT copyfile (XCUS, 0,0,0,null.chx,  XITX, 0,0,0,null.chx)) FAIL;

   /*** Edit the temporary file. */
   if (NOT unit_lock  (XITX, WRITE, L(0), L(0), L(0), L(0), null.chx)) FAIL;
   pass = editfile (XITX, thisuser->editor, (int) thisuser->truncate);
   unit_unlk (XITX);
   if (NOT pass)                                     FAIL;

   /*** Load the edited temporary file, and parse/reformat it as needed.
   /    Write the reformatted version back to XCUS. */
   ul0    = chxalloc (L(200), THIN, "cus_user ul0");
   priorg = chxalloc (L(30), THIN, "cususer priorg");
   load_ulist  (XITX, confnum, ul0, priorg, &errors);
   store_ulist (confnum, XCUS, ul0, priorg);
   chxfree (ul0);
   chxfree (priorg);

   /*** Kill the temporary file. */
   if (unit_lock (XITX, WRITE, L(0), L(0), L(0), L(0), null.chx)) {
       unit_kill (XITX);
       unit_unlk (XITX);
   }
   
   /*** Distribute the updated userlist, if appropriate. */
   if (is_distributed (confnum)  &&
       unit_lock (XCUS, READ, L(confnum), L(0), L(0), L(0), null.chx)) {
      ctext = chxalloc (L(500), THIN, "cus_user ctext");
      chixfile  (XCUS, ctext, null.str);
      unit_unlk (XCUS);
      cusxusers (confnum, ctext, nullchix, &errors);
      chxfree   (ctext);
   }

   /*** If the organizer just edited the conference userlist, parse it and
   /    complain if something is wrong (like the organizer no longer able
   /    to join the conference!) */
   groups = nlnode (1);
   allowed_on (nullchix, null.nl, -1);          /* Flush Ulist cache. */
   if (allowed_on (thisuser->id, groups, confnum) <= co_priv_str ("include"))
      mdwrite (XWER, "mai_Torgallow", null.md);
   nlfree (groups);

 done:

   RETURN  (success);
}
