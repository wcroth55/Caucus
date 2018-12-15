/*** A_ADD_PERSON.   Add a new person (ie. register him)
/
/    ok = a_add_person (person, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Person    person;   (object to add)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Add (store) a new person.  Usually this is done at registration
/       or pre-registration.
/
/    How it works:
/       Uses storereg() to store PERSON into the Caucus DB.
/       Currently it ignores MASK.
/
/    Returns: A_OK on success
/             A_BADARG on bad argument
/             A_NOPERSON if userid isn't specified
/             A_NOTNEW if userid already registered
/             A_DBERR on database error
/             A_TOOMANY if too many users already registered
/
/    Error Conditions:
/      If more than one property is requested, if any one of them has
/      a database error, a_get_conf() returns A_DBERR -- even if the
/      rest of the properties are ok.
/  
/    Side effects: 
/
/    Related functions: a_mak_person, a_fre_person, a_get_person
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/aaddpers.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV 12/03/92 12:17 New function. */
/*: JV 12/08/92 12:51 Call decode_setopts to parse the settings, rm userid. */
/*: CR 12/22/92 15:51 Last arg to chxnextline() is int4. */
/*: CR  1/26/93 15:49 Use PLAIN and strplain() to decrypt license info. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */
/*: CR  5/17/95 18:10 Add (userid) part to newly created person's name. */
/*: CR 10/30/95 12:30 Add A_TOOMANY return for too many total users. */
/*: CR 12/22/03 Do not mark newly created user as "on now". */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

extern Chix   newuseropts, confid;

FUNCTION  a_add_person (Person person, int4 mask, int wait)
{
   short    c;
   int      success, error, total;
   int4     n;
   Userreg  reg, make_userreg();
   Namelist add, del, nlnode(), nlnames();
   Chix     temp, name;

   if (disk_failure (0, &error, NULL))                      return (A_DBERR);

   if (person == (Person) NULL  ||  person->tag != T_PERSON)  return (A_BADARG);
   if (EMPTYCHX (person->owner))                            return (A_NOPERSON);

   person->ready = 0;

   name   = chxalloc (L(80), THIN, "aaddpers name");
   temp   = chxalloc (L(80), THIN, "aaddpers temp");
   reg = make_userreg();
   reg->briefs = nlnode (1);
   reg->print  = nlnode (1);

   /*** First check to see if user exists already. */
   if (sysudnum (person->owner, -1, 0) > 0 &&
       loadreg  (person->owner, reg, person->sitevar)) DONE (A_NOTNEW);

   /*** Create USERnnn and user dir if necessary. */
   sysdirchk (person->owner);

   /*** Create the user's personal mail directory. */
   if (NOT unit_lock (XUMD, WRITE, L(0), L(0), L(0), L(0), person->owner))
      DONE (A_DBERR);
   if (NOT unit_make (XUMD))          { unit_unlk (XUMD);  DONE (A_DBERR); }
   unit_write (XUMD, CQ("0\n"));
   unit_close (XUMD);
   unit_unlk  (XUMD);

   /*** Create this user by filling in a Userreg and storing it. */
   success = A_OK;
   chxcpy    (reg->id, person->owner);
   chxclear  (reg->name);
   chxformat (reg->name, CQ("%s (%s)"), L(0), L(0), person->name, person->owner);
   chxcpy    (reg->lastname, person->lastname);
   chxcpy    (reg->phone, person->phone);

   for (n=0; chxnextline (temp, person->intro->data, &n); ) 
     nlcat (reg->briefs, temp, 0);

   chxcpy (reg->setopts, person->setopts);
   decode_setopts (reg, reg->setopts, nullchix, newuseropts);
   chxcpy (reg->setopts, person->laston);

   if (NOT storereg (reg, 0, person->sitevar))  DONE (A_DBERR);

   /*** Add each word in this user's full name to the global nameparts
   /    directory. */
   del = nlnode  (1);
   chxpunct (name, reg->name);
   add = nlnames (name, person->owner);

   c = modnames (XSND, XSNN, nullchix, 0, L(0), add, del, NOPUNCT);
   nlfree (add);
   nlfree (del);
   if (NOT c)  DONE (A_DBERR);

   /*** If distributed, write change name transaction.  (What should be
   /    done if chgxperson() returns an error?) */
   /*
   if (PLAIN(license.function_mask) & FM_DIST) 
      chgxperson (reg->name, nullchix, person->owner, reg->phone, &error);
   */

done:
   chxfree (name);
   chxfree (temp);
   free_userreg (reg);
   person->ready = 1;
   return (success);
}
