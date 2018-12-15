
/*** FREE_PARTIC.   Deallocate a "partic" structure made by make_partic().
/
/    free_partic (partic);
/
/    Parameters:
/       struct partic_t *partic;     (ptr to partic struct)
/
/    Purpose:
/       Deallocate the heap memory used by PARTIC.
/
/    How it works:
/
/    Returns: 1 
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:  make_partic().
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  resp/freepart.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/13/93 13:35 New function. */
 
#include <stdio.h>
#include "caucus.h"

FUNCTION  free_partic (partic)
   struct partic_t *partic;
{
   ENTRY ("free_partic", "");

   sysfree ( (char *) partic->responses);
   sysfree ( (char *) partic->backresp);
   chxfree (          partic->lastin);

   RETURN  (1);
}
