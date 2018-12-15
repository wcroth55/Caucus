
/*** ORG_CONFIRM.  Does the Organizer confirm changing this?
/
/    ORG_CONFIRM is called when the user tries to change something
/    (item, response) that does not beint4 to him.  It returns
/    1 if the user is the Organizer and confirms that he really
/    wants make the change, and returns 0 otherwise.
/
/    User may choose:
/      YES    go ahead and change it
/      NO     no, don't change, go on 
/      STOP   no, don't change, cease all changing for now.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:34 Source code master starting point */
/*: CR 11/09/88 13:05 Handle <CANCEL> while changing another's response. */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CR  7/13/90 14:36 Use thisuser.is_organizer. */
/*: DE  3/17/92 12:02 Chixified */
/*: CR  2/09/95 16:31 Add "stop" to list of choices. */

#include "caucus.h"
#include <stdio.h>

extern Userreg                thisuser;
extern union  null_t          null;
extern Chix                   yesnotable;

FUNCTION  org_confirm()
{
   Chix   answer, def, choices;
   short  s, success;

   ENTRY ("orgconfirm", "");

   answer    = chxalloc (L(20), THIN, "orgconfi answer");
   def       = chxalloc (L(20), THIN, "orgconfi def"); 
   choices   = chxalloc (L(40), THIN, "orgconfi choices"); 
   success   = 0;

   if (NOT thisuser->is_organizer)  FAIL;

   mdstr    (choices, "chg_Aorgcon", null.md);
   chxtoken (def, null.chx, 1, choices);
   mdwrite  (XWTX, "chg_Torgcon", null.md);
   s = mdprompter ("chg_Porgcon", choices, 2, "chg_Horgcon", 
                                  10, answer, def);
   if  (s <= -3  ||  s==2)  sysbrkset();
   DONE(s == 1);

done:
   chxfree ( choices );
   chxfree ( answer );
   chxfree ( def );

   RETURN  ( success );
}
