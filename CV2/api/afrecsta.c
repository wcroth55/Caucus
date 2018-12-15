/*** A_FRE_CSTAT.   Free (deallocate) a Cstat object.
/
/    a_fre_cstat (cstat);
/
/    Parameters:
/       Cstat     cstat;   (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by a_mak_cstat().
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if cstat is null or not an Cstat.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_cstat()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afrecstat.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/15/92 17:47 New function. */
/*: CR 12/01/92 14:46 Cast sysfree () argument. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  a_fre_cstat (Cstat cstat)
{
   if (cstat == (Cstat) NULL)   return (0);
   if (cstat->tag != T_CSTAT)   return (0);

   if (api_debug) a_debug (A_FREE, (Ageneric) cstat);
   chxfree (cstat->owner);
   cstat->tag = 0;

   sysfree ( (char *) cstat->resps);
   sysfree ( (char *) cstat);

   return  (1);
}
