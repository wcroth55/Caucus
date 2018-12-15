/*** FREE_USERREG.   Free a Userreg allocated by make_userreg().
/
/    free_userreg (u);
/
/    Parameters:
/       Userreg  u;     (Userreg to be freed)
/
/    Purpose:
/       Free a Userreg allocated by make_userreg().
/
/    How it works:
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/freeuser.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/16/91 21:39 New function. */
/*: CP  8/17/93 14:12 Free 'terminal' member. */
/*: JV  9/07/93 11:33 Free 'transfer'. */

#include <stdio.h>
#include "caucus.h"

extern union null_t  null;

FUNCTION   free_userreg (u)
   Userreg u;
{
   ENTRY ("free_userreg", "");

   if (u == nulluser)  RETURN (1);

   chxfree (u->name);
   chxfree (u->lastname);
   chxfree (u->id);
   chxfree (u->setopts);
   chxfree (u->sysopts);
   chxfree (u->phone);
   chxfree (u->laston);
   chxfree (u->editor);
   chxfree (u->eot);
   chxfree (u->startmenu);
   chxfree (u->terminal);
   chxfree (u->transfer);

   nlfree (u->briefs);
   nlfree (u->print);

   RETURN (1);
}
