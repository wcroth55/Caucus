/*** LOADREG.   Load USER's registration file into REG.
/
/    ok = loadreg (user, reg, sitevar);
/
/    Parameters:
/       int     ok;               (Success?)
/       Chix    user;             (userid of user)
/       Userreg reg;              (user registration info struct)
/       Chix    sitevar[];        (array of memory variables for this user)
/       
/    Purpose:
/       Load the known registration information about userid USER into
/       REG.   Load the user's memory variables into SITEVAR.
/
/    How it works:
/       If USER is a userid on this host, load their registration file
/       into REG, and their memory variables into SITEVAR.  (If SITEVAR
/       is null, do not load the memory variables.)
/
/       If USER is on a remote host, copy USER into the userid field of
/       REG, and clear the rest of the fields.
/
/    Returns: 1 on success
/             0 on any error condition
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/loadreg.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: JV  9/12/88 14:44 Changed AOS diagnostics to print only on debug.*/
/*: JV  9/08/89 12:31 Added validity check on input parm. */
/*: CW  6/08/89 11:07 Use bugtell() instead of printf(). */
/*: CW  9/12/89 16:47 Declare bugtell(). */
/*: JV  8/06/90 19:55 Add Site-Dependent Reg Info. */
/*: CR  3/05/91 22:39 Use REG_MAX... and loadline() to limit string size.*/
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  7/24/91 22:25 Comment out loadline() def, it's in getperso.c. */
/*: CR  7/30/91 18:11 Add new sitevar argument. */
/*: CR 11/06/91 10:56 Handle remote ids, and header comments. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: CR  1/07/92 14:44 Use new decode_setopts(). */
/*: CR  1/09/92 13:19 Add sys opt stuff. */
/*: DE  4/13/92 13:35 Chixified */
/*: DE  6/01/92 17:00 Patched unit_read */
/*: JV 12/03/92 15:03 Save setopts and sysopts lines in reg. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR 10/12/01 16:40 Ignore @'s!  Replace with "^" for now. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;
extern int          debug;
extern Chix         newuseropts;

FUNCTION  loadreg (user, reg, sitevar)
   Chix    user;
   Userreg reg;
   Chix    sitevar[];
{
   struct namlist_t *nlnode();
   Chix   line, sys, id, host, myhost, tempchx;
   char  *bugtell();
   short  i;
   int    success;

   ENTRY ("loadreg", "");

   if (NOT chxtype(user)) RETURN (0);  /* Validity Check on input parameter. */

   line     = chxalloc (L(200), THIN, "loadreg line");
   sys      = chxalloc (L(160), THIN, "loadreg sys");
   id       = chxalloc (L(80),  THIN, "loadreg id");
   host     = chxalloc (L(80),  THIN, "loadreg host");
   myhost   = chxalloc (L(80),  THIN, "loadreg myhost");
   success  = 0;

   nlfree (reg->briefs);
   nlfree (reg->print);
   reg->briefs = reg->print = null.nl;

   /*** If USER is a user on a remote host, set up REG's id and clear
   /    everything else. */
   chxbreak  (user, id, host, CQ("^")); 
   getmyhost (myhost);
   if (NOT EMPTYCHX(host)  &&  NOT chxeq (host, myhost)) {
      chxcpy (reg->id, user);
      chxclear(reg->name);
      chxclear(reg->lastname);
      chxclear(reg->phone);
      chxclear(reg->laston);
      SUCCEED;
   }

   if (NOT unit_lock (XURG, READ, L(0), L(0), L(0), L(0), id)) {
      if (debug) bugtell ("Loadreg: Couldn't lock XURG", 0, L(0), "");
      FAIL;
   }
   if (NOT unit_view (XURG)) {
      if (debug) bugtell ("Loadreg: Couldn't view XURG", 0, L(0), "");
      unit_unlk (XURG);
      FAIL;
   }

   chxcpy   (reg->id, id);
   loadline (XURG, reg->name,    REG_MAX2);
   if (debug) bugtell ("Got user's name as:",      0, L(0), ascquick(reg->name));
   loadline (XURG, reg->lastname, REG_MAX1);
   if (debug) bugtell ("Got user's last name as: ", 0, L(0), 
                       ascquick(reg->lastname));
   unit_read (XURG, line, L(0));
   chxcpy (reg->setopts, line);
   unit_read (XURG, sys, L(0));
   chxcpy (reg->sysopts, sys);
   decode_setopts (reg, line, sys, newuseropts);
   loadline  (XURG, reg->phone,   REG_MAX2);
   loadline  (XURG, reg->laston,  REG_MAX2);

   reg->briefs = nlnode(1);

   /* Read Site-Dependent Reg Info if there is any. */
   unit_read (XURG, line, L(0));
   if (chxvalue(line,0L) == '=')           /* First check for SDRI Preface. */
      for (i=0;   i < 10;   i++) {
         unit_read (XURG, line, L(0));
         if (sitevar != NULLSITE) {
            chxcpy (sitevar[i], tempchx = CHXSUB(line,1L)); /* Skip over
               space in first column. */
            chxfree ( tempchx );
	  }
      }
   else if (chxvalue(line,0L) != '+') {  /* This is Pre-CV2.4 Brief Intro. */
      nlcat (reg->briefs, tempchx = CHXSUB(line,1L), 0);  /* (Can't push 
             line back.) */
      chxfree ( tempchx );
    }
    
   if (chxvalue(line,0L) != '+')        /* Takes care of Pre- and Post-CV2.4.*/
      while (unit_read (XURG, line, L(0))) {
         if (chxvalue(line,0L) == '+')  break;
         nlcat (reg->briefs, tempchx = CHXSUB(line,1L), 0);
         chxfree ( tempchx );
      }

   reg->print = nlnode(1);
   while (unit_read (XURG, line, L(0)))  nlcat (reg->print, line, 0);

   unit_close (XURG);
   unit_unlk  (XURG);
   SUCCEED;

 done:

   chxfree ( line );
   chxfree ( sys );
   chxfree ( id );
   chxfree ( host );
   chxfree ( myhost );

   RETURN ( success );
}
/*
FUNCTION  loadline (n, what, max)
   int    n, max;
   char  *what;
{
   char   temp[200];
   int    success;
   ENTRY  ("loadline", "");

   success = unit_read (n, temp, L(0));
   temp[max-1] = '\0';
   strcpy (what, temp);
   RETURN (success);
}
*/
