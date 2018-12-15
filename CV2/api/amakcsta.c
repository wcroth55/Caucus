/*** A_MAK_CSTAT.   Make (allocate) a Cstat object.
/
/    cstat = a_mak_cstat (name);
/
/    Parameters:
/       Cstat   cstat;    (returned/created object)
/       char   *name;     (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize a Cstat object.
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
/    Related functions:  a_fre_cstat()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amakcsta.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/30/92 17:29 New function. */
/*: CR 11/30/92 23:28 Raise to MAXITEMS+5, for safety. */
/*: CR 12/01/92 14:46 Cast sysfree () argument. */
/*: CP  8/01/93  9:37 Add 'name' argument, debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "maxparms.h"
#include "api.h"

extern int api_debug;

FUNCTION  Cstat  a_mak_cstat (char *name)
{
   Cstat  cstat;

   cstat = (Cstat) sysmem (sizeof (struct Cstat_t), "Cstat_t");
   if (cstat == (Cstat) NULL)  return (cstat);

   cstat->tag    = T_CSTAT;
   A_SET_NAME (cstat->dname, name);
   cstat->cnum   = -1;
   cstat->items  = -1;
   cstat->time   =  0;
   cstat->resps  = (short *) sysmem (sizeof(short) * (MAXITEMS + 5), "resps");
   cstat->owner  = chxalloc (L(24), THIN, "a_mak_cstat owner");
   cstat->ready  =  0;

   if (cstat->resps == (short *) NULL) {
      sysfree ( (char *) cstat);
      cstat = (Cstat) NULL;
   }
   else if (api_debug) a_debug (A_ALLOC, (Ageneric) cstat);

   return (cstat);
}
