
/*** VARTAB_IS.   Is this variable in the var tab?
/
/    slot = vartab_is (vt, name);
/   
/    Parameters:
/       int     slot;   (returned slot number in VARS)
/       Vartab  vt;     (variable table)
/       Chix    name;   (variable name)
/
/    Purpose:
/       Is variable NAME in the variable table VT?
/
/    How it works:
/
/    Returns: slot number on success
/             -1   if not found
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  api/vartis.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/11/98 12:21 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "null.h"

FUNCTION  vartab_is (Vartab vt, Chix name)
{
   int    num;

   /*** Scan all of the allocated variables first... */
   for (num=0;   num<vt->used;   ++num) {

      /*** Found a matching name? */
      if (chxeq (vt->name[num], name))        return (num);
   }

   return (-1);
}
