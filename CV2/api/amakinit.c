/*** A_MAK_INIT.   Make (allocate) an Initapi object.
/
/    initapi = a_mak_init (name);
/
/    Parameters:
/       Initapi   initapi   (returned/created object)
/       char     *name;     (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize an Initapi object.
/
/    How it works:
/
/    Returns: newly created object
/             NULL on error
/
/    Error Conditions: insufficient heap memory, or heap catastrophe.
/  
/    Side effects: 
/
/    Related functions:  a_fre_init()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amakinit.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/12/92 17:47 New function. */
/*: CR  1/19/93 13:11 Add license, remove userid member. */
/*: CK  3/01/93 14:40 Provide initial value for 'tbufs' member. */
/*: JV  6/09/93 12:00 Add terminal. */
/*: CP  8/01/93  9:37 Add 'name' argument, debugging stuff. */
/*: CR  3/28/95  7:52 Add userid member. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  Initapi  a_mak_init (char *name)
{
   Initapi  init;

   init = (Initapi) sysmem (sizeof (struct Initapi_t), "initapi");
   if (init == (Initapi) NULL)  return (init);

   init->tag       = T_INIT;
   A_SET_NAME (init->dname, name);
   init->confid    = chxalloc (L(80), THIN, "a_mak_in confid");
   init->tbufs     = 3;
   init->terminal  = 0;
   init->diskform  = 0;
   init->userid[0] = '\0';

   if (api_debug) a_debug (A_ALLOC, (Ageneric) init);

   return (init);
}
