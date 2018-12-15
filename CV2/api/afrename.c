/*** A_FRE_NAMEX.   Free (deallocate) a Namex object.
/
/    a_fre_namex (namex);
/
/    Parameters:
/       Namex     namex;   (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by a_mak_namex().
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if namex is null or not an Namex.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_namex()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afrenamex.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/15/93 21:21 New function. */
/*: CP  8/01/93  9:37 Add debugging stuff. */
/*: CK  8/02/93 12:32 Cast 2nd arg to a_debug(). */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  a_fre_namex (Namex namex)
{

   if (namex == (Namex) NULL)  return (0);
   if (namex->tag != T_NAMEX)  return (0);

   if (api_debug) a_debug (A_FREE, (Ageneric) namex);
   namex->tag = 0;

   chxfree (namex->owner);
   chxfree (namex->namepart);
   a_fre_rlist (namex->rlist);

   sysfree ( (char *) namex);

   return (1);
}
