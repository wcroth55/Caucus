/*** MAKE_USERREG.   Allocate and initialize a Userreg.
/
/    u = make_userreg();
/
/    Parameters:
/       Userreg  u;     (returned userreg struct)
/
/    Purpose:
/       Create a new, empty user registration structure.
/
/    How it works:
/
/    Returns: a new userreg_t struct
/             NULL on error
/
/    Error Conditions:
/       Insufficient memory, or the heap is screwed up.
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
/    Home:  people/makeuser.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/16/91 21:39 New function. */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: CR  2/26/93 16:41 Clear export_mail. */
/*: CP  8/17/93 14:11 Add 'showattach', 'terminal' members. */
/*: JV  9/07/93 11:34 Add 'transfer'. */

#include <stdio.h>
#include "caucus.h"

extern union null_t  null;

FUNCTION   Userreg make_userreg()
{
   Userreg u;
   char   *sysmem();

   ENTRY ("make_userreg", "");

   u = (Userreg) sysmem (L(sizeof (struct userreg_T)), "make_userreg");
   if (u == nulluser)  RETURN (u);

   u->name       = chxalloc (L(80), THIN, "make_user name");
   u->lastname   = chxalloc (L(40), THIN, "make_user lastname");
   u->id         = chxalloc (L(20), THIN, "make_user id");
   u->setopts    = chxalloc (L(60), THIN, "make_user setopts");
   u->sysopts    = chxalloc (L(10), THIN, "make_user sysopts");
   u->phone      = chxalloc (L(40), THIN, "make_user phone");
   u->laston     = chxalloc (L(40), THIN, "make_user laston");
   u->editor     = chxalloc (L(20), THIN, "make_user editor");
   u->eot        = chxalloc (L(20), THIN, "make_user eot");
   u->startmenu  = chxalloc (L(20), THIN, "make_user startmenu");
   u->terminal   = chxalloc (L(20), THIN, "make_user terminal");
   u->transfer   = chxalloc (L(20), THIN, "make_user transfer");

   u->briefs     = null.nl;
   u->print      = null.nl;
   u->export_mail = 0;
   u->showattach  = 0;

   RETURN (u);
}
