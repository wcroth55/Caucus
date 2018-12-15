/*** A_MAK_PSTAT.   Make (allocate) a Pstat object.
/
/    pstat = a_mak_pstat (name);
/
/    Parameters:
/       Pstat   pstat;    (returned/created object)
/       char   *name;     (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize a Pstat object.
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
/    Related functions:  a_fre_pstat()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amakpsta.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/01/92 13:48 New function. */
/*: CR 12/01/92 14:46 Cast sysfree () argument. */
/*: CP  8/01/93  9:37 Add 'name' argument, debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "maxparms.h"
#include "api.h"

extern int api_debug;

FUNCTION  Pstat  a_mak_pstat (char *name)
{
   Pstat  pstat;

   pstat = (Pstat) sysmem (sizeof (struct Pstat_t), "pstat");
   if (pstat == (Pstat) NULL)  return (pstat);

   pstat->tag    = T_PSTAT;
   A_SET_NAME (pstat->dname, name);
   pstat->cnum   = -1;
   pstat->owner  = chxalloc (L(20), THIN, "a_mak_ps owner");
   pstat->items  = 0;
   pstat->resps  = (short *) sysmem (sizeof(short) * (MAXITEMS + 5), "resps");
   pstat->back   = (short *) sysmem (sizeof(short) * (MAXITEMS + 5), "back");
   pstat->lastin = chxalloc (L(30), THIN, "a_mak_ps lastin");
   pstat->ready  = 0;

   if (pstat->resps == (short *) NULL  ||  pstat->back == (short *) NULL) {
      if (pstat->resps != (short *) NULL)  sysfree ( (char *) pstat->resps);
      if (pstat->back  != (short *) NULL)  sysfree ( (char *) pstat->back);
      sysfree ( (char *) pstat);
      pstat = (Pstat) NULL;
   }
   else if (api_debug) a_debug (A_ALLOC, (Ageneric) pstat);

   return (pstat);
}
