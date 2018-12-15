
/*** FREE_ULIST.   Deallocate a userlist created by make_ulist().
/
/    free_ulist (u);
/
/    Parameters:
/       Chix     *u;       (userlist to free)
/
/    Purpose:
/       Deallocate a userlist created y make_ulist().
/
/    How it works:
/
/    Returns: 1
/
/    Error Conditions
/  
/    Side effects:
/
/    Related functions:
/
/    Called by: 
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  ulist/freeulis.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/22/91 22:54 New function. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  free_ulist (u)
   Chix  *u;
{
   int    i;

   ENTRY ("free_ulist", "");

   for (i=0;   i<MAXULIST;   ++i)    if (u[i] != nullchix)  chxfree (u[i]);

   sysfree ( (char *) u);

   RETURN  (1);
}
