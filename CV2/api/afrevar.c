/*** A_FRE_VAR.   Free (deallocate) a Var object.
/
/    a_fre_var (var);
/
/    Parameters:
/       Var     var;   (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by a_mak_var().
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if var is null or not a Var.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_var()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afrevar.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/06/95 12:31 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  a_fre_var (Var var)
{
   int index;

   if (var      == (Var) NULL)  return (0);
   if (var->tag !=  T_VAR)      return (0);

   if (api_debug) a_debug (A_FREE, (Ageneric) var);
   chxfree    (var->owner);
   chxfree    (var->vname);
   chxfree    (var->value);

   sysfree ( (char *) var);

   return  (1);
}
