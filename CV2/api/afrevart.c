/*** A_FRE_VARTAB.   Free (deallocate) a Vartab object.
/
/    a_fre_vartab (vartab);
/
/    Parameters:
/       Vartab   vartab;    (object to be freed)
/
/    Purpose:
/       Deallocate a Vartab object allocated by a_mak_vartab.
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if obect is null or not a Vartab

/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_vartab()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afrevart.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/09/96 15:47 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "null.h"

extern union null_t null;
extern int          api_debug;

FUNCTION  a_fre_vartab (Vartab vartab)
{
   int    slot;

   if (vartab == (Vartab) NULL)  return (0);
   if (vartab->tag != T_VARTAB)  return (0);

   if (api_debug) a_debug (A_FREE, (Ageneric) vartab);
   vartab->tag      = 0;

   chxfree (vartab->owner);
   for (slot=0;   slot < vartab->used;   ++slot) {
      if (vartab->name[slot] != nullchix) {
         chxfree (vartab->name [slot]);
         chxfree (vartab->value[slot]);
      }
   }
   sysfree ( (char *) vartab->name);
   sysfree ( (char *) vartab->value);
   sysfree ( (char *) vartab->code);
   sysfree ( (char *) vartab);

   return (1);
}
