
/*** FREE_CML.   Free a created CML_File object. 
/
/    free_cml (cf);
/   
/    Parameters:
/       CML_File   cf;
/
/    Purpose:
/       Release all memory used by a CML_File object.
/
/    How it works:
/
/    Returns:
/
/    Home:  sweb/freecml.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/22/98 23:08 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  free_cml (CML_File cf)
{
   CML_File  new;
   int       num;
   char     *sysmem();

   ENTRY ("free_cml", "");

   for (num=0;   cf->lines[num] != (Chix) NULL;   ++num)  
      chxfree (cf->lines[num]);

   sysfree ( (char *) cf);

   RETURN (1);
}
