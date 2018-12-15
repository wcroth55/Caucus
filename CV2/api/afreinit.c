/*** A_FRE_INIT.   Free (deallocate) an Initapi object.
/
/    a_fre_init (initapi);
/
/    Parameters:
/       Initapi   initapi;  (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by a_mak_init().
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if init is null or not an Initapi.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_init()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afreinit.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/12/92 17:47 New function. */
/*: CR  1/19/93 13:12 Remove userid member. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  a_fre_init (Initapi init)
{
   if (init == (Initapi) NULL)  return (0);
   if (init->tag != T_INIT)     return (0);

   if (api_debug) a_debug (A_FREE, (Ageneric) init);
   init->tag = 0;

   chxfree (init->confid);
   sysfree ( (char *) init);

   return  (1);
}
