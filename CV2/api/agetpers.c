/*** A_GET_person.   Get basic information about a person.
/
/    ok = a_get_person (person, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Person    person;   (object to fill with person info)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Load basic information about a person into the PERSON object.
/
/    How it works:
/       Uses loadreg() and XURG to get the
/       requested properties into PERSON.
/
/    Returns: A_OK on success
/             A_BADARG on bad argument
/             A_NOPERSON if user doesn't exist
/             A_DBERR on database error
/
/    Error Conditions:
/      If more than one property is requested, if any one of them has
/      a database error, a_get_conf() returns A_DBERR -- even if the
/      rest of the properties are ok.
/  
/    Side effects: 
/
/    Related functions: a_mak_person, a_fre_person
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/agetpers.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV 12/03/92 11:17 New function. */
/*: CR  3/20/95 18:57 Add use of P_LASTNM. */
/*: CR  3/30/95 13:52 Clear intro stuff first! */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

extern int             debug;
extern union  null_t   null;

FUNCTION  a_get_person (Person person, int4 mask, int wait)
{
   static   Chix empty = (Chix) NULL;
   int      success;
   Userreg  reg, make_userreg();
   Namelist nl, nlnode();

   if (person == (Person) NULL  ||  person->tag != T_PERSON)  return (A_BADARG);
   if (EMPTYCHX (person->owner))                              return (A_BADARG);

   if (empty == (Chix) NULL)  empty = chxalloc (L(12), THIN, "a_get_p empty");
   person->ready = 0;

   reg = make_userreg();
   reg->briefs = nlnode (1);
   reg->print  = nlnode (1);
   if (NOT sysudnum (person->owner, -1, 0) > 0 ||
       NOT loadreg  (person->owner, reg, person->sitevar)) DONE (A_NOPERSON);

   success = A_OK;
   if (mask & P_OWNER)    chxcpy   (person->owner,    reg->id);
   if (mask & P_NAME)     removeid (person->name, reg->name, reg->id, empty);
   if (mask & P_LASTNM)   chxcpy   (person->lastname, reg->lastname);
   if (mask & P_PHONE)    chxcpy   (person->phone,    reg->phone);
   if (mask & P_TEXT)   {
      chxclear  (person->intro->data);
      person->intro->lines = 0;
      for (nl = reg->briefs->next; nl != null.nl; nl = nl->next) {
         chxcat (person->intro->data, nl->str);
         chxcat (person->intro->data, CQ("\n"));
         person->intro->lines++;
      }
   }
   if (mask & P_SETTING)  chxcpy (person->setopts, reg->setopts);
   if (mask & P_LASTON)   chxcpy (person->laston,  reg->laston);
   if (mask & P_PRINT) 
      for (nl = reg->print->next; nl != null.nl; nl = nl->next) {
         chxcat (person->print->data,    nl->str);
         person->print->lines++;
      }

done:
   free_userreg (reg);
   person->ready = 1;
   return (success);
}
