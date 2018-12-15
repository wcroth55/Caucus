
/*** FREE_ADDRESS.   Free an Address created by make_address().
/
/    free_address (a);
/
/    Parameters:
/       Address   a;      (old Address to be freed)
/
/    Purpose:
/       Free an Address previously created by calling make_address().
/
/    How it works:
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions: make_address(), into_address()
/
/    Called by:  Caucus (addmess()...)
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/freeaddr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 11/11/91 23:58 New function. */
/*: JX  5/27/92 13:16 Cast sysfree arg. */

#include <stdio.h>
#include "caucus.h"

FUNCTION   free_address (a)
   Address a;
{
   ENTRY ("free_address", "");

   chxfree (a->name);
   chxfree (a->id);
   chxfree (a->host);
   sysfree ((char *) a);

   RETURN  (1);
}
