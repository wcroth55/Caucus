
/*** STORE_ULIST.   Store a conference userlist 'ulist' into a file.
/
/    ok = store_ulist (cnum, u, ulist, priorg);
/
/    Parameters:
/       int       ok;         (success?)
/       int       cnum;       (conference number)
/       int       u;          (unit number of file; usually XCUS)
/       Chix      ulist;      (write this userlist)
/       Chix      priorg;     (primary organizer)
/
/    Purpose:
/       Store a conference userlist ULIST into a file.
/
/    How it works:
/
/    Returns: 1 on success.
/             0 if file cannot be written, or other error.
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
/    Home:  ulist/storeulis.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/30/91 16:42 New function. */
/*: CR 10/31/91 12:26 Fix double ;;. */
/*: CR 10/31/91 12:36 Check each ulist[index] for nullchix. */
/*: CR 11/06/91 11:14 Decl strtoken(). */
/*: UL 12/11/91 18:41 Make unit_lock args 3-6 int4.*/
/*: DE  3/17/92 16:57 Chixified */
/*: JX  5/13/92 11:45 Removed xtra vars. */
/*: CR 12/07/92 15:12 Add new U argument; chxfree word2! */
/*: CL 12/11/92 11:13 Long Chxcatsub args. */
/*: CR 10/08/97 17:09 Simplify! */

#include <stdio.h>
#include "caucus.h"
#define  LOG_FILE  4

FUNCTION  store_ulist (cnum, u, ulist, priorg)
   int    cnum, u;
   Chix   ulist, priorg;
{
   int    success;

   ENTRY ("store_ulist", "");

   /*** Open the userlist file for writing. */
   if (NOT unit_lock (u, WRITE, L(cnum), L(0), L(0), L(0), nullchix)) FAIL;
   if (NOT unit_make (u))                    { unit_unlk (u);   FAIL; }

   unit_write (u, CQ(":primary "));
   unit_write (u, priorg);
   unit_write (u, CQ("\n"));

   unit_write (u, ulist);

   unit_close (u);
   unit_unlk  (u);

   SUCCEED;

 done:
   RETURN  ( success );
}
