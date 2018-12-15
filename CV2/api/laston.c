/*** LAST_ON.   Set when the current user was "last on".
/
/    last_on (on);
/
/    Parameters:
/       int       on;       (1 means on now; 0 means off now)
/
/    Purpose:
/       Update the date/time the current user was "last on", and
/       record whether that user is on now (or just "off") now.
/
/    How it works:
/
/    Returns: 
/
/    Error Conditions:
/  
/    Called by:  a_init(), a_exit()
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/laston.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/11/95 16:56 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "caucus.h"

FUNCTION  last_on (int on)
{
   Userreg  reg, make_userreg();
   Namelist nlnode();
   extern   Chix sitevar[];
   Chix     id;

   reg = make_userreg();
   reg->briefs = nlnode (1);
   reg->print  = nlnode (1);

   id = chxalloc (L(40), THIN, "last_on id");
   sysuserid (id);

   /*** First check to see if user exists already. */
   if (NOT sysudnum (id, -1, 0) > 0 ||
       NOT loadreg  (id, reg, sitevar))   return;

   storereg (reg, on, sitevar);

   chxfree (id);
   free_userreg (reg);

   return;
}
