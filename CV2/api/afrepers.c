/*** A_FRE_PERSON.   Free (deallocate) a Person object.
/
/    a_fre_person (person);
/
/    Parameters:
/       Person     person;   (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by a_mak_person().
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if person is null or not a Person.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_person()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afrepers.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV 12/03/92 11:00 New function. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  a_fre_person (Person person)
{
   int index;

   if (person == (Person) NULL)    return (0);
   if (person->tag != T_PERSON)    return (0);

   if (api_debug) a_debug (A_FREE, (Ageneric) person);
   chxfree    (person->owner);
   chxfree    (person->name);
   chxfree    (person->lastname);
   chxfree    (person->phone);
   a_fre_text (person->intro);
   a_fre_text (person->print);
   chxfree    (person->setopts);
   chxfree    (person->laston);
   for (index=0;   index<10;   index++)
      chxfree (person->sitevar[index]);

   sysfree ( (char *) person);

   return  (1);
}
