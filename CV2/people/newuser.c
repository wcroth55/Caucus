
/*** NEW_USER.   Add this user as a new user on this conference.
/
/    Register this user, and create his/her user file for this conference.
/    Returns 1 if the user registers properly, and 0 otherwise.  The User's
/    participation file is already locked, and should stay that way. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: CR  1/30/89 12:40 Skip INTRO text if user hit <CANCEL>. */
/*: JV  9/19/89 17:26 Add _new_user AFM. */
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: CR 10/17/89 15:17 Replace 2nd arg of FIRE_ARM with "". */
/*: JV  3/27/90 12:50 Removed unnecessary declaration of screen. */
/*: CR  8/12/90 18:43 New calling sequence for tomember(). */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: CX  3/17/92 15:44 Replace FIRE_AFM with fire_afm(). */
/*: DE  4/14/92 14:50 Chixified */
/*: DE  6/01/92 17:00 Patch unit_read */
/*: JX  6/06/92 15:16 Add 3rd arg to unit_read. */
/*: CR 10/11/92 22:09 Change comments. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */
/*: CR  8/20/04 Add XUPA arg to store_user() call. */

#include <stdio.h>
#include "caucus.h"

extern Userreg                 thisuser;
extern union  null_t           null;
extern Chix                    yesnotable;
extern Chix                    afm_name;
extern short                   confnum;

FUNCTION  new_user (thisconf)
   struct partic_t *thisconf;
{
   struct namlist_t *add, *del, *nlnode();
   Chix   str, nextconf, newstr;
   short  i;
   int    success;

   ENTRY ("new_user", "");

   str      = chxalloc (L(80), THIN, "newuser str");
   nextconf = chxalloc (L(40), THIN, "newuser nextconf");
   newstr   = chxalloc (L(80), THIN, "newuser newstr");
   success  = 0;

   /*** Tell the user who should join this conference. */
   unit_write (XWTX, NEWCPAGE);
   unit_lock  (XCIN, READ, L(confnum), L(0), L(0), L(0), null.chx);
   if (unit_view (XCIN) ) {
      sysbrkclear();
      while (unit_read (XCIN, newstr, L(0))) {
         if (sysbrktest())  break;
         unit_write (XWTX, newstr);
         unit_write (XWTX, CQ("\n"));
      }
      sysbrkclear();
      unit_close(XCIN);
   }
   unit_unlk (XCIN);

   /*** Ask the user if he/she really wants to join? */
   if (mdprompter ("new_Pjoin", yesnotable, 2, "new_Hjoin", 20, str, null.chx)
              <= 0)  FAIL;


   /*** Create user's participation file for this conference. */
   thisconf->items = 0;
   for (i=0;  i<MAXITEMS;     ++i)   thisconf->responses[i] = -1;
   store_user (thisconf, XUPA);


   /*** Add this user to the membership list for this conference. */
   tomember (str, thisuser->lastname, thisuser->name, thisuser->id);
   add = nlnode(1);
   del = nlnode(1);
   nladd  (add, str, 0);
   modnames (XCMD, XCMN, null.chx, confnum, L(0), add, del, NOPUNCT);
   nlfree (add);
   nlfree (del);


   /*** If this is the organizer, we may want to initialize some
   /    special things here. */

   /*** And if it's not, we may want to send the greeting message
   /    to the user.   Version 1 used to have code for this, but
   /    it has been removed. */

   /* Now fire off the AFM. */
   fire_afm ("_new_user", null.chx, nextconf);
   
   SUCCEED;

 done:

   chxfree ( str );
   chxfree ( nextconf );
   chxfree ( newstr );

   RETURN ( success );
}
