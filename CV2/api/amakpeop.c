/*** A_MAK_PEOPLE.   Make (allocate) a People object.
/
/    people = a_mak_people (name);
/
/    Parameters:
/       People   people;    (returned/created object)
/       char    *name;      (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize a People object.
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
/    Related functions:  a_fre_people()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amakpeop.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV 12/14/92 15:31 New function. */
/*: CR  1/13/93 14:26 Remove unused 'index'. */
/*: CP  8/01/93  9:37 Add 'name' argument, debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "null.h"

extern union null_t null;
extern int          api_debug;

FUNCTION  People  a_mak_people (char *name)
{
   People people;

   people = (People) sysmem (sizeof (struct People_t), "people");
   if (people == (People) NULL)  return (people);

   people->tag      = T_PEOPLE;
   A_SET_NAME (people->dname, name);
   people->namepart = chxalloc (L(32), THIN, "amakpeop namepart");
   people->cnum     = 0;
   people->ids      = null.nl;

   people->ready = 0;

   if (api_debug) a_debug (A_ALLOC, (Ageneric) people);

   return (people);
}
