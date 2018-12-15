/*** A_FRE_RLIST.   Free (deallocate) an Rlist object.
/
/    a_fre_rlist (rlist);
/
/    Parameters:
/       Rlist     rlist;   (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by a_mak_rlist().
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if rlist is null or not an Rlist.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_rlist()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afrerlis.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/15/93 23:28 New function. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  a_fre_rlist (Rlist rlist)
{
   Rlist  save;

   if (rlist == (Rlist) NULL)  return (0);

   while (rlist != (Rlist) NULL) {
      if (api_debug) a_debug (A_FREE, (Ageneric) rlist);
      save = rlist->next;
      sysfree ( (char *) rlist);
      rlist = save;
   }

   return (1);
}
