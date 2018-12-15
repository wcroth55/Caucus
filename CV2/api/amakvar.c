/*** A_MAK_VAR.   Make (allocate) a Var object.
/
/    var = a_mak_var (name);
/
/    Parameters:
/       Var      var;       (returned/created object)
/       char    *name;      (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize a Var object.
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
/    Related functions:  a_fre_var()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amakvar.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/06/95 12:28 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  Var  a_mak_var (char *name)
{
   Var    var;

   var = (Var) sysmem (sizeof (struct Var_t), "Var_t");
   if (var == (Var) NULL)  return (var);

   var->tag    = T_VAR;
   var->cnum   = 0;
   var->owner  = chxalloc (L(32), THIN, "a_mak_pe owner");
   var->vname  = chxalloc (L(64), THIN, "a_mak_pe vname");
   var->value  = chxalloc (L(80), THIN, "a_mak_pe setopts");
   var->ready  = 0;

   if (api_debug) a_debug (A_ALLOC, (Ageneric) var);

   return (var);
}
