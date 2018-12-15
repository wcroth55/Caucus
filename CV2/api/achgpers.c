/*** A_CHG_PERSON.   Change a person's properites
/
/    ok = a_chg_person (person, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Person    person;   (object to change)
/       int4      mask;     (mask of properties to change)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Change registration information of a new person.
/
/    How it works:
/       Uses loadreg() to get info on PERSON.  Then changes
/       relevant data, and stores changed data with storereg().
/
/       Use one of the following values for MASK:
/          P_NAME    Change full name
/          P_LASTNM  Change last name
/          P_PHONE   Change phone number
/          P_TEXT    Change personal intro
/          P_SETTING Change values of SET command
/
/    Returns: A_OK on success
/             A_BADARG on bad argument
/             A_NOPERSON if userid isn't specified or doesn't exist
/             A_DBERR on database error
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
/    Home:  api/achgpers.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV 12/03/92 13:17 New function. */
/*: JV 12/14/92 12:49 Change membership list in confs, add P_SETTING. */
/*: CR 12/22/92 16:14 Last arg to chxnextline() is int4. */
/*: CR  1/13/93 14:51 Decl sysmem(), make arg int4; cast sysfree arg. */
/*: CR  1/26/93 15:49 Use PLAIN and strplain() to decrypt license info. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */
/*: CR  5/17/95 23:41 Include "(userid)" part of name... */
/*: CR 12/22/03 Do not automatically mark user as "on now". */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

extern Chix             sitevar[], newuseropts;
extern Chix             ss_luid, ss_ruid;

FUNCTION  a_chg_person (Person person, int4 mask, int wait)
{
   short    c;
   int      success, error, i;
   int4     n;
   Userreg  reg, make_userreg();
   Namelist add, del, nlnode(), nlnames();
   Chix     temp, name, member, oldlast, oldname;
   struct   pdir_t pdir;

   if (disk_failure (0, &error, NULL))                      return (A_DBERR);

   if (person == (Person) NULL  ||  person->tag != T_PERSON)  return (A_BADARG);
   if (EMPTYCHX (person->owner))                            return (A_NOPERSON);

   person->ready = 0;

   name    = chxalloc (L(80), THIN, "achgpers name");
   temp    = chxalloc (L(80), THIN, "achgpers temp");
   member  = chxalloc (L(16), THIN, "achgpers member");
   oldlast = chxalloc (L(16), THIN, "achgpers oldlast");
   oldname = chxalloc (L(16), THIN, "achgpers oldname");

   reg = make_userreg();
   reg->briefs = nlnode (1);
   reg->print  = nlnode (1);

   /*** First check to see if user exists already. */
   if (NOT sysudnum (person->owner, -1, 0) > 0 ||
       NOT loadreg  (person->owner, reg, person->sitevar)) DONE (A_NOPERSON);

   /*** Create this user by filling in a Userreg and storing it. */
   success = A_OK;
   
   if ((mask & P_NAME) || (mask & P_LASTNM)) {
      /* Allocate and initialize necessary data structures. */
      pdir.type  = -1;
      pdir.parts =  0;
      pdir.resp  = (short *) sysmem (sizeof(short) * L(PDIRMAX  + 3), "p.r");
      pdir.lnum  = (int4  *) sysmem (sizeof(int4 ) * L(PDIRMAX  + 3), "p.l");

      /* First save the old info. */
      chxcpy (oldlast, reg->lastname);
      chxcpy (oldname, reg->name);
 
      if (mask & P_LASTNM)  chxcpy (reg->lastname, person->lastname);
      if (mask & P_NAME) {
         chxcpy (reg->name, person->name);
         chxcat (reg->name, CQ(" "));
         chxcat (reg->name, ss_luid);
         chxcat (reg->name, reg->id);
         chxcat (reg->name, ss_ruid);
     }

      /*** Add each word in this user's full name to the global nameparts
      /    directory. */
      if (mask & P_NAME) {
         chxpunct (name, oldname)  ; del = nlnames (name, reg->id);
         chxpunct (name, reg->name); add = nlnames (name, reg->id);

         c = modnames (XSND, XSNN, nullchix, 0, L(0), add, del, NOPUNCT);
         nlfree (add);
         nlfree (del);
   
         if (NOT c)  {
            sysfree ( (char *) pdir.resp);
            sysfree ( (char *) pdir.lnum);
            DONE (A_DBERR);
         }
      }

      /*** Change this user's name in the membership list of every conference
      /    the user belongs to. (!)  */
      tomember (member, oldlast, oldname, reg->id);
      del = nlnode(1);   nladd (del, member, 0);
      tomember (member, person->lastname, reg->name, reg->id);
      add = nlnode(1);   nladd (add, member, 0);

      syspdir (&pdir, XUPA, 0, 0, reg->id);
      for (i=0;   i<pdir.parts;   ++i) {
         unit_wipe (LCNF);
         modnames (XCMD, XCMN, nullchix, pdir.resp[i], L(0), add, del, NOPUNCT);
      }

      nlfree  (add);
      nlfree  (del);
      sysfree ( (char *) pdir.resp);
      sysfree ( (char *) pdir.lnum);

      /*** If distributed, write change name transaction.  (What should be
      /    done if chgxperson() returns an error?) */
      /* if (PLAIN(license.function_mask) & FM_DIST) 
         chgxperson (reg->name, nullchix, reg->id, reg->phone, &error);
      */
   }
   
   if (mask & P_PHONE)  chxcpy (reg->phone, person->phone);
   if (mask & P_SETTING) {
      chxcpy (reg->setopts, person->setopts);
      decode_setopts (reg, reg->setopts, nullchix, newuseropts);
   }

   if (mask & P_TEXT)  {
      nlfree (reg->briefs);
      reg->briefs = nlnode (1);
      for (n=0 ; chxnextline (temp, person->intro->data, &n); )
        nlcat (reg->briefs, temp, 0);
   }


   if (NOT storereg (reg, 0, person->sitevar))  DONE (A_DBERR);

done:
   chxfree (name);
   chxfree (temp);
   free_userreg (reg);
   person->ready = 1;
   return (success);
}
