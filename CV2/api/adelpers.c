/*** A_DEL_PERSON.   Completely remove a person from Caucus.
/
/    ok = a_del_person (person, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Person    person;   (object to delete)
/       int4      mask;     (ignored)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Remove a person who doesn't use Caucus anymore.
/
/    How it works:
/       Uses person->owner to identify person.
/
/    Returns: A_OK on success
/             A_BADARG on bad argument
/             A_NOPERSON if userid isn't specified or doesn't exist
/             A_DBERR on database error
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions: a_mak_person, a_fre_person, a_get_person, a_chg_person
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/adelpers.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  8/25/93 13:17 New function. */
/*: CK  9/22/93 18:36 Add 'item' arg to modnames() call. */
/*: CR  3/21/95 16:13 Fix return codes(!), add A_NOPERSON return. */

#include <stdio.h>
#include "common.h"
#include "caucus.h"
#include "unitwipe.h"
#include "api.h"

FUNCTION a_del_person (Person person, int4 mask, int wait)
{
   int     c, i, error, confnum, success;
   Chix    member;
   struct  namlist_t *add, *del, *users, *nlnode(), *nlnames();
   Userreg who, make_userreg();
   static  int first=1;
   static  struct pdir_t pdir;

   if (disk_failure (0, &error, NULL))             return (A_DBERR);

   /* Various allocations and resettings. */
   member = chxalloc (L(16), THIN, "killuser name");
   who = make_userreg();

   pdir.type  = -1;
   pdir.parts = 0;
   if (first) {
      pdir.resp = (short *) sysmem (sizeof(short) * L(PDIRMAX  + 3), "p.r");
      pdir.lnum = (int4  *) sysmem (sizeof(int4 ) * L(PDIRMAX  + 3), "p.l");
      first = 0;
   }

   /*** Remove the person's name from the master name files. */
   if (NOT loadreg (person->owner, who, NULLSITE))  DONE (A_NOPERSON);
   add = nlnode  (1);
   del = nlnames (who->name, who->id);
   c = modnames (XSND, XSNN, nullchix, 0, L(0), add, del, NOPUNCT);
   nlfree (del);
   if (NOT c)  DONE (A_DBERR); 

   /*** Let the network know we removed this person. */              
   chgxperson (nullchix, who->name, who->id, nullchix, &error);
 
   /*** Remove the person's name from the membership list of every
   /    conference s/he belongs to. */
   chxformat (who->name, CQ(" (%s)"), L(0), L(0), who->id, nullchix);
   tomember  (member, who->lastname, who->name, who->id);
   del = nlnode(1);   nladd (del, member, 0);
   syspdir (&pdir, XUPA, 0, 0, who->id);
   for (i=0;   i<pdir.parts;   ++i) {
      confnum = pdir.resp[i];
      modnames (XCMD, XCMN, nullchix, (int) confnum, L(0), add, del, NOPUNCT);
   }
   nlfree (del);
    
   /*** Remove the person from the captive-user password file, if there. */
   del = nlnode(1);
   nladd (del, who->id, 0);
   c = matchnames (XSPD, XSPN, 0, nullchix, del, AND, &users, NOPUNCT);
   nlfree (del);
   if (c) {
      if (nlsize (users) == 1)
         modnames (XSPD, XSPN, nullchix, (int) confnum, L(0), add, users, NOPUNCT);
      nlfree (users);
   }
   nlfree (add);
 
   /*** Remove the user's USER and TEXT directories and contents. */
   unit_wipe (LUSR);
   syskill (who->id);
   DONE (A_OK);

done:
   chxfree (member);
   free_userreg(who);

   return (success);
}
