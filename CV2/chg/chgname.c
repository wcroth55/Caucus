
/*** CHG_NAME.   Change user's full name.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:33 Source code master starting point */
/*: AM  2/07/89 10:25 Jcaucus changes*/
/*: CR  4/27/89 14:16 Detect new name that is only blanks. */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CR  8/12/90 15:10 Add "(userid)" part to new full name.  Use chg_full(). */
/*: CR  8/29/90 14:30 Remove unused stuff. */
/*: CX 11/10/91 18:08 Chixify. */
/*: JX  5/19/92 12:45 Fix Chix. */
/*: CR 10/11/92 21:49 Remove CQ from (md)prompter() call. */
/*: JV 12/04/92 13:38 Add sitevar arg to storereg(). */
/*: CR  7/07/94 15:46 Remove (userid) from old name before display. */

#include <stdio.h>
#include "caucus.h"

extern Userreg              thisuser;
extern union  null_t        null;
extern Chix                 ss_luid, ss_ruid, sitevar[];

FUNCTION  chg_name()
{
   Chix   newname, oldname, oldlast;
   short  c;
   int    success;

   ENTRY ("chgname", "");

   newname = chxalloc (L(80), THIN, "chg_name newname");
   oldname = chxalloc (L(80), THIN, "chg_name oldname");
   oldlast = chxalloc (L(80), THIN, "chg_name oldlast");
   success = 0;

   /*** Save the user's old name. */
   chxcpy (oldname, thisuser->name);
   chxcpy (oldlast, thisuser->lastname);

   /*** Display user's current name. */
   unit_write (XWTX, NEWCPAGE);
   mdwrite    (XWTX, "chg_Toldname", null.md);
   removeid   (newname, thisuser->name, thisuser->id, oldlast);
   unit_write (XWTX, newname);

   /*** Get the new name from the user. */
   while (1) {
      c = mdprompter ("chg_Pnewname", null.chx, 0, "chg_Hnewname", 
                       50, newname, null.chx);
      if (c <= -3)  goto done;
      if (c == -2)  continue;

      chxsimplify  (newname);
      if (NOT EMPTYCHX (newname))  break;
   }

   /*** Set up the user's new name, and add the "(userid)" part. */
   chxcpy  (thisuser->name, newname);
   if (NOT lastname (thisuser))  goto done;
   chxcat (thisuser->name, CQ(" "));
   chxcat (thisuser->name, ss_luid);
   chxcat (thisuser->name, thisuser->id);
   chxcat (thisuser->name, ss_ruid);

   /*** Actually change the name in the database. */
   chg_full (oldname, oldlast, thisuser->name, thisuser->lastname);

   /*** Finally, rewrite the user's registration file. */
   storereg (thisuser, 1, sitevar);
   success = 1;

done:
   chxfree (newname);
   chxfree (oldname);
   chxfree (oldlast);
   RETURN  (success);
}
