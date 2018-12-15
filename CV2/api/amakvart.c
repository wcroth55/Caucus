/*** A_MAK_VARTAB.   Make (allocate) a Vartab object.
/
/    vt = a_mak_vartab (name);
/
/    Parameters:
/       Vartab   vt;        (returned/created object)
/       char    *name;      (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize a Vartab object.
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
/    Related functions:  a_fre_vartab()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amakvart.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/09/96 15:39 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "null.h"

#define  INITIAL_SIZE  200

extern union null_t null;
extern int          api_debug;

FUNCTION  Vartab  a_mak_vartab (char *name)
{
   Vartab vt;
   int    slot;

   vt = (Vartab) sysmem (sizeof (struct Vartab_t), "Vartab_t");
   if (vt == (Vartab) NULL)  return (vt);

   vt->tag     = T_VARTAB;
   A_SET_NAME (vt->dname, name);
   vt->id1     = vt->id2 = 0;

   vt->owner = chxalloc (L(20), THIN, "a_mak_vartab owner");
   vt->name  = (Chix *)  sysmem (sizeof(Chix)  * INITIAL_SIZE, "name");
   vt->value = (Chix *)  sysmem (sizeof(Chix)  * INITIAL_SIZE, "value");
   vt->code  = (short *) sysmem (sizeof(short) * INITIAL_SIZE, "code");
   if (vt->value == NULL)   return (NULL);

   for (slot=0;   slot<INITIAL_SIZE;   ++slot) {
      vt->name [slot] = nullchix;
      vt->value[slot] = nullchix;
      vt->code [slot] = -1;
   }
   vt->max  = INITIAL_SIZE;
   vt->used = 0;
   

   if (api_debug) a_debug (A_ALLOC, (Ageneric) vt);

   return (vt);
}
