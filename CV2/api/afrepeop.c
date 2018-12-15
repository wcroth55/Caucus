/*** A_FRE_PEOPLE.   Free (deallocate) a People object.
/
/    a_fre_people (people);
/
/    Parameters:
/       People   people;    (object to be freed)
/
/    Purpose:
/       Deallocate a People object allocated by a_mak_people.
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if obect is null or not a People

/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_people()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afrepeop.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV 12/14/92 15:29 New function. */
/*: CR  1/13/93 14:23 Remove unused 'index'. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "null.h"

extern union null_t null;
extern int          api_debug;

FUNCTION  a_fre_people (People people)
{

   if (people == (People) NULL)  return (0);
   if (people->tag != T_PEOPLE)  return (0);

   if (api_debug) a_debug (A_FREE, (Ageneric) people);
   people->tag      = 0;

   chxfree (people->namepart);
   if (people->ids  != null.nl) nlfree (people->ids);

   sysfree ( (char *) people);

   return (1);
}
