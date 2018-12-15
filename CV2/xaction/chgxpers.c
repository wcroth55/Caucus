/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CHGXPERSON. Create a xaction to publicize a new person/changed person
/
/   int n = chgxperson (newname, oldname, user_id, phone, error)
/
/   Parameters:
/      Chix  newname;    (user's new name)
/      Chix  oldname;    (user's old name)
/      Chix  user_id;    (userid on local host)
/      Chix  phone;      (user's telephone #)  (may be nullchix!)
/      int  *error;
/
/   Purpose:
/      NOTE: THIS FUNCTION IS NOT SUPPOSED TO DO ANYTHING YET.
/      It is an accumulation of code towards what should happen
/      once we starting handling "people" information over CaucusLink.
/
/      Caucus_x should call this to generate a xaction so that the
/      new information about this person will be broadcast to all
/      hosts that are accepting this type of information.
/
/   How it works:
/      A new person has their name in newname, and an empty ("") oldname.
/      An existing person who is changing their name should have both an
/      oldname and a newname.
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/
/   Error Conditions:
/
/   Related Functions:
/
/   Called by:
/
/   Home: xaction/chgxperson.c
/ */

/*: JV  6/05/91 12:42 Create this. */
/*: JV  6/10/91 16:12 Change arg list, add code. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/22/91 16:32 Add ENTRY */
/*: CR  7/28/91 17:11 Add oldname argument. */
/*: JV  7/29/91 12:58 Change name from addxperson -> chgxperson. */
/*: JV  7/29/91 13:06 Actually write the code. */
/*: JV  7/30/91 08:44 Actually set the ERROR arg, declare systime. */
/*: JV 12/07/91 21:21 Return without doing anything. */
/*: DE  6/02/92 16:56 Chixified */
/*: JX  9/01/92 12:48 Change empty_xact to make_xaction(). */
/*: CR 10/11/92 22:45 Allow PHONE to be nullchix. */
/*: CP  7/16/93 17:13 Add comments. */

#include "handicap.h"
#include <stdio.h>
#include "chixuse.h"
#include "done.h"
#include "entryex.h"
#include "derr.h"
#include "unitcode.h"
#include "xaction.h"

FUNCTION chgxperson (newname, oldname, user_id, phone, error)
   Chix newname, oldname, user_id, phone;
   int  *error;
{
   Xactionptr xact, make_xaction();
   int4   systime();
   Chix   mychn, date, time;
   int    success;
   char  *strcpy();
   ENTRY ("chgxperson", "");

   RETURN (0);

   mychn    = chxalloc (L(80), THIN, "chgxpers mychn");
   date     = chxalloc (L(40), THIN, "chgxpers date");
   time     = chxalloc (L(20), THIN, "chgxpers time");
   success  = 0;

   xact = make_xaction();

   if (NOT get_chn (mychn, XSNF, 0)) {
      bug ("CHGXPERSON: can't get XSNF.", 0); *error=DNOCHN; FAIL; }

   xact->typenum = TT_PERSON;

   chxformat (xact->uid, CQ("%s@%s"), L(0), L(0), user_id, mychn);
   chxcpy (xact->uid, user_id);
   if (EMPTYCHX(oldname)) chxcpy (xact->uname, oldname);
   chxcpy   (xact->unewname, newname);
   chxclear (xact->phone);
   if (phone != nullchix)  chxcpy (xact->phone, phone);

   sysdaytime (date, time, 0, systime());
   chxformat (xact->date, CQ("%s %s"), L(0), L(0), date, time);

   FAIL;
done:
   chxfree (mychn);
   chxfree (date);
   chxfree (time);
   free_xaction (xact);

   RETURN (success);
}   
