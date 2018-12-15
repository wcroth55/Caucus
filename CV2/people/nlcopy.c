
/*** NLCOPY.   Produce a copy of a namelist.
/
/    new = nlcopy (old);
/
/    Parameters:
/       Namelist  new;       (resulting copy)
/       Namelist  old;       (original to be copied)
/
/    Purpose:
/       Produce an exact copy of an existing namelist.
/
/    How it works:
/
/    Returns: a new namelist, or NULL on memory allocation failure.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies: None.
/
/    Known bugs:
/
/    Home:  people/nlcopy.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/02/91 12:24 New function. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  Namelist nlcopy (old)
   Namelist  old;
{
   Namelist  new, p, nlnode();

   ENTRY ("nlcopy", "");

   if ( (new = nlnode (1)) == NLNULL)  RETURN (NLNULL);

   for (p=old->next;   p!=NLNULL;   p=p->next)   nlcat (new, p->str, p->num);

   RETURN (new);
}
