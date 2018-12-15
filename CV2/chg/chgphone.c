
/*** CHG_PHONE.   Change user's phone number. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:33 Source code master starting point */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CX 11/10/91 18:28 Chixify. */
/*: CR 10/11/92 21:49 Remove CQ from (md)prompter() call. */
/*: CR 12/10/92 14:51 JV changed storereg() call, no carvmark(?) */

#include <stdio.h>
#include "caucus.h"

extern Userreg           thisuser;
extern union  null_t     null;
extern Chix   sitevar[];

FUNCTION  chg_phone()
{
   int    choice;

   ENTRY ("chgphone", "");

   unit_write (XWTX, NEWCPAGE);
   mdwrite    (XWTX, "chg_Toldphone", null.md);
   unit_write (XWTX, thisuser->phone);
   choice = mdprompter ("chg_Pphone", null.chx, 0, "chg_Hphone", 20, 
                         thisuser->phone, null.chx);

   if (choice <= -3)  RETURN (1);

   storereg (thisuser, 1, sitevar);
   RETURN (1);
}
