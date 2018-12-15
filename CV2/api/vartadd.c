
/*** VARTAB_ADD.   Add a new value for a variable to a variable table.
/
/    slot = vartab_add (vt, name, value);
/   
/    Parameters:
/       int     slot;     (returned slot # in table)
/       Vartab  vt;       (variable table)
/       Chix    name;     (variable name)
/       Chix    value;    (new value for variable)
/
/    Purpose:
/       Change the value of variable NAME to VALUE.  (If NAME does
/       not already exist in the variable list VARS, first add it.)
/
/    How it works:
/
/    Returns: location (slot #) of NAME.
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  api/vartadd.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/09/96 16:03 New function (based on old sweb/varadd) */
/*: CR  2/05/02 11:00 Add return value. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "null.h"

FUNCTION  int vartab_add (Vartab vt, Chix name, Chix value)
{
   int    slot;

   slot = vartab_find (vt, name);
   if (slot >= 0)  chxcpy (vt->value[slot], value);

   return (slot);
}
