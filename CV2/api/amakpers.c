/*** A_MAK_PERSON.   Make (allocate) a Person object.
/
/    person = a_mak_person (name);
/
/    Parameters:
/       Person   person;    (returned/created object)
/       char    *name;      (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize a Person object.
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
/    Related functions:  a_fre_person()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amakpers.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV 12/03/92 10:31 New function. */
/*: CP  8/01/93  9:37 Add 'name' argument, debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  Person  a_mak_person (char *name)
{
   int    index;
   Person person;

   person = (Person) sysmem (sizeof (struct Person_t), "person");
   if (person == (Person) NULL)  return (person);

   person->tag      = T_PERSON;
   A_SET_NAME (person->dname, name);
   person->owner    = chxalloc (L(32), THIN, "a_mak_pe owner");
   person->name     = chxalloc (L(64), THIN, "a_mak_pe name");
   person->lastname = chxalloc (L(16), THIN, "a_mak_pe lastname");
   person->phone    = chxalloc (L(32), THIN, "a_mak_pe phone");
   person->intro    = a_mak_text();
   person->setopts  = chxalloc (L(80), THIN, "a_mak_pe setopts");
   person->laston   = chxalloc (L(16), THIN, "a_mak_pe laston");
   for (index=0;   index<10;   index++) 
      person->sitevar[index] = chxalloc (L(100), THIN, "sitevar");
   person->print    = a_mak_text();

   person->ready = 0;

   if (api_debug) a_debug (A_ALLOC, (Ageneric) person);

   return (person);
}
