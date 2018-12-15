
/*** MAKE_PARTIC.   Allocate an item/response "partic" structure.
/
/    ok = make_partic (partic, maxitems);
/
/    Parameters:
/       int              ok;         (success?)
/       struct partic_t *partic;     (ptr to unallocated partic struct)
/       int              maxitems;   (max number of items)
/
/    Purpose:
/       A "partic" structure has members that must be allocated off the
/       heap before the structure can be used.  Make_partic() allocates
/       and initializes these members to allow for MAXITEMS items.
/
/    How it works:
/
/    Returns: 1 on success
/             0 if any memory allocation failed.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:  free_partic().
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  resp/makepart.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/13/93 13:30 New function. */
 
#include <stdio.h>
#include "caucus.h"
#include "sysexit.h"

FUNCTION  make_partic (partic, maxitems)
   struct partic_t *partic;
   int    maxitems;
{
   char *sysmem();

   ENTRY ("make_partic", "");

   partic->responses = (short *) sysmem (L(sizeof(short)) * L(maxitems + 3), "partic.resp");
   partic->backresp  = (short *) sysmem (L(sizeof(short)) * L(maxitems + 3), "partic.back");
   partic->lastin    = chxalloc (L(20), THIN, "make_part lastin");

   if (partic->responses == (short *) NULL  ||
       partic->backresp  == (short *) NULL)  /*  RETURN (0); */
          sysexit (EXIT_MEM);

   RETURN (1);
}
