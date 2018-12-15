/*** GET_PERSON.  Object oriented version of LOADREG.
/
/   int n = get_person (user, reg)
/
/   Parameters:
/      Chix    user;
/      Userreg reg;
/
/   Purpose:
/      Load registration information about a person.
/
/   How it Works:
/      Just like LOADREG(), except it caches the reg struct for subsequent
/      calls.  Whenever a piece of information is desired by an application
/      program, the program should call the appropriate function (e.g.
/      get_u_dicti(), to get the user's dictionary number) which then
/      calls get_person().  get_person() returns the requested info
/      in REG.
/
/   Returns:
/      1 on success
/      0 on failure.  If the caller (or caller's caller) has verified
/         that this person exists, then this return implies an error
/         in the database.
/
/   Related functions: get_u_dicti()
/
/   Home: people/getperso.c
/
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  7/16/91 09:44 Create function. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: CR  1/07/92 15:21 Use new decode_setopts(). */
/*: CR  1/09/92 13:43 Use new sys opt stuff. */
/*: DE  4/10/92 14:47 Chixified */
/*: JX  5/27/92 13:15 Fixify. */
/*: JV 12/08/92 12:50 Don't use nullchix for defstr of decode_setopts(). */
/*: CL 12/10/92 17:06 Long ascofchx args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: JV  5/05/93 15:01 Fix caching mechanism, other bugs. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;
extern int          debug;
extern Chix         sitevar[], newuseropts;

FUNCTION  get_person (user, reg)
   Chix     user;
   Userreg  reg;
{
   struct namlist_t *nlnode();
   Chix   line, sys, tempchx;
   char   tempchar[100], *bugtell();
   short  i;
   Userreg make_userreg();
   static Userreg  myreg = (Userreg) NULL;
   int    success;

   ENTRY ("get_person", "");

   if (NOT chxtype(user)) RETURN (0);  /* Validity Check on input parameter. */

   line      = chxalloc (L(200), THIN, "getperson line");
   sys       = chxalloc (L(160), THIN, "getperson sys");
   success   = 0;

   if (myreg == (Userreg) NULL) myreg = make_userreg();

   if (chxeq (myreg->id, user)) {      /* Return if already loaded. */
      *reg = *myreg;
      SUCCEED;
   }

   nlfree (reg->briefs);
   nlfree (reg->print);
   reg->briefs = reg->print = null.nl;

   if (NOT unit_lock (XURG, READ, L(0), L(0), L(0), L(0), user)) {
      if (debug) bugtell ("get_person: Couldn't lock XURG", 0, L(0), "");
      FAIL;
   }
   if (NOT unit_view (XURG)) {
      if (debug) bugtell ("get_person: Couldn't view XURG", 0, L(0), "");
      unit_unlk (XURG);
      FAIL;
   }

   chxclear(reg->id);          /* Fill in at end to ensure data integrity */

   loadline (XURG, reg->name,    REG_MAX2);
   if (debug) {
      ascofchx (tempchar, reg->name, L(0), L(98));
      bugtell ("Got user's name as:",       0, L(0), tempchar);
   }
   loadline (XURG, reg->lastname, REG_MAX1);
   if (debug) {
      ascofchx (tempchar, reg->lastname, L(0), L(98));
      bugtell ("Got user's last name as: ", 0, L(0), tempchar);
   }
   unit_read (XURG, line, L(0));
   unit_read (XURG, sys, L(0));
   decode_setopts (reg, line, sys, newuseropts);
   loadline (XURG, reg->phone,   REG_MAX2);
   loadline (XURG, reg->laston,  REG_MAX2);

   reg->briefs = nlnode(1);

   /* Read Site-Dependent Reg Info if there is any. */
   unit_read (XURG, line, L(0));
   if (chxvalue(line,0L) == '=')           /* First check for SDRI Preface. */
      for (i = 0; i < 10; i++) {
         unit_read (XURG, line, L(0));
         chxcpy (sitevar[i], tempchx = CHXSUB(line,1L));
            /* Skip over space in first column. */
         chxfree (tempchx);
      }
   else if (chxvalue(line,0L) != '+') {   /* This is Pre-CV2.4 Brief Intro. */
                                                 /* (Can't push line back.) */
      nlcat (reg->briefs, tempchx = CHXSUB(line,1L), 0); 
      chxfree (tempchx);
   }
   if (chxvalue(line,0L) != '+')     /* Takes care of Pre- and Post-CV2.4.   */
      while (unit_read (XURG, line, L(0))) {
         if (chxvalue(line,0L) == '+')  break;
         nlcat (reg->briefs, tempchx = CHXSUB(line,1L), 0);
         chxfree (tempchx);
      }

   reg->print = nlnode(1);
   while (unit_read (XURG, line, L(0)))  nlcat (reg->print, line, 0);

   unit_close (XURG);
   unit_unlk  (XURG);

   chxcpy   (reg->id, user);
   *myreg = *reg;

   SUCCEED;

 done:
   chxfree (line);
   chxfree (sys);

   RETURN (success);
}

FUNCTION  loadline (n, what, max)
   int    n, max;
   Chix   what;
{
   Chix   temp, other;
   int    success;
   ENTRY  ("loadline", "");

   temp     = chxalloc (L(200), THIN, "loadline temp");

   success = unit_read (n, temp, L(0));
   other = chxalsub (temp, L(0), L(max-1));
   chxcpy  (what, other);
   chxfree (temp);
   chxfree (other);
   RETURN  (success);
}
