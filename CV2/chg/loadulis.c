
/*** LOAD_ULIST.   Load a conference userlist into a ulist.
/
/    ok = load_ulist (u, cnum, ulist, priorg, errors);
/
/    Parameters:
/       int       ok;         (success?)
/       int       u;          (unit number containing userlist file)
/       int       cnum;       (conference number)
/       int      *errors;     (number of errors detected)
/       Chix      ulist;      (put processed userlist here)
/       Chix      priorg;     (primary organizer)
/
/    Purpose:
/       Load the conference userlist from the file U for conference
/       CNUM into ULIST.
/
/    How it works:
/       Load_ulist() scans the userlist file U, and parses both old-style
/       (":include") and new-style (":1.1 include") control lines.
/
/       If Load_ulist() can not understand a control line, it comments
/       it out (and preceeds it with a comment that the next line is
/       in error).  Load_ulist() sets ERROR to the number of such errors
/       encountered.
/
/       Note: load_ulist() assumes that ULIST is empty.  If ULIST is not
/       empty, the contents of file U are *added* *to* what is in ULIST.
/
/    Returns: 1 on success.
/             0 if conference does not exist, or other error.
/
/    Error Conditions
/  
/    Side effects:
/
/    Related functions:
/
/    Called by: 
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  ulist/loadulis.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/22/91 22:54 New function. */
/*: CR 10/31/91 14:19 Fix a variety of bugs. */
/*: CR 11/06/91 11:14 Decl strscan(). */
/*: CR 11/15/91 13:38 Add host to data entries missing "@host". */
/*: CR 11/18/91 11:40 Fix special case of index==0. */
/*: UL 12/11/91 18:41 Make unit_lock args 3-6 int4.*/
/*: JX  5/19/92 20:52 Fix Chix (no Chix carve!). */
/*: JX  9/23/92 16:52 Fixify. */
/*: CR 12/07/92 15:56 Expand comments. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR 10/08/97 16:23 Simplify to just ":keywords" (again!) */
/*: CR 04/13/04 Use co_priv() to get priv nums. */

#include <stdio.h>
#include "caucus.h"

#define  LOG_FILE  4

extern union null_t null;

FUNCTION  load_ulist (u, cnum, ulist, priorg, errors)
   int    u, cnum, *errors;
   Chix   ulist, priorg;
{
   int    type, success;
   int4   pos, sub;
   Chix   line, word;
   char   temp[100];

   ENTRY ("load_ulist", "");

   line    = chxalloc (L(160), THIN, "loadulis line");
   word    = chxalloc (L(100), THIN, "loadulis word");
   success = 0;
   chxclear (priorg);

   /*** Open the userlist file. */
   *errors = 0;
   if (NOT unit_lock (u, READ, L(cnum), L(0), L(0), L(0), null.chx))   FAIL;
   if (NOT unit_view (u))                     { unit_unlk (u);   FAIL; }

   while (unit_read (u, line, L(0))) {

      if (chxvalue (line, L(0)) == ':') {
         if (cf_digit ((int) chxvalue (line, L(1)))) {
            chxtoken (word, nullchix, 2, line);
            chxconc  (line, CQ(":"), word);
         }
         
         jixreduce (line);
         chxtoken  (word, null.chx, 1, line);
/*       type = tablematch (inextab, word); */
         type = co_priv (word);

         if (chxeq (word, CQ(":primary"))) {
            chxtoken (priorg, null.chx, 2, line);
            continue;
         }
      
         else if (type < 0)  chxconc (line, CQ("#"), line);
      }

      chxcat    (ulist, line);
      chxcatval (ulist, THIN, L('\n'));
   }

   unit_close (u);
   unit_unlk  (u);

   /*** Patch in primary organizer if not already in userlist. */
   if (chxvalue (priorg, L(0)) == 0) {
      if (unit_lock (XCMA, READ, L(cnum), L(0), L(0), L(0), nullchix)) {
         if (unit_view (XCMA)) {
            unit_read  (XCMA, priorg, L(0));
            unit_close (XCMA);
         }
         unit_unlk (XCMA);
      }
   }

   SUCCEED;
   
done:

   chxfree ( line );
   chxfree ( word );

   RETURN  (success);
}
