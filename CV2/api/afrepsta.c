/*** A_FRE_PSTAT.   Free (deallocate) a Pstat object.
/
/    a_fre_pstat (pstat);
/
/    Parameters:
/       Pstat     pstat;   (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by a_mak_pstat().
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if pstat is null or not an Pstat.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_pstat()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afrepsta.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/01/92 13:54 New function. */
/*: CR 12/01/92 14:46 Cast sysfree () argument. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  a_fre_pstat (Pstat pstat)
{
   if (pstat == (Pstat) NULL)   return (0);
   if (pstat->tag != T_PSTAT)   return (0);

   if (api_debug) a_debug (A_FREE, (Ageneric) pstat);
   pstat->tag = 0;

   chxfree (pstat->owner);
   chxfree (pstat->lastin);
   sysfree ( (char *) pstat->resps);
   sysfree ( (char *) pstat->back);
   sysfree ( (char *) pstat);

   return  (1);
}
