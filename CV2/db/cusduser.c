/*** CUS_DUSERS.   Apply a received userlist transaction to the database.
/
/    ok = cus_dusers (cnum, ctext, priorg, error);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    cnum;        (conference number)
/       Chix   ctext;       (text of userlist transaction)
/       Chix   priorg;      (primary organizer)
/       int   *error;       (return error code)
/
/    Purpose:
/       Cus_dusers() applies the text of a received userlist transaction
/       to the database for a given conference on this host.
/
/    How it works:
/       Apply the changes in CTEXT to conference CNUM.
/
/    Returns: 1 on success, sets ERROR to 0.
/             0 on any error condition, see below.
/
/    Error Conditions: sets ERROR accordingly:
/       DNOCONF     conference CNUM does not exist
/  
/    Side effects:
/
/    Related functions:
/
/    Called by: XFU
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/cusduser.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/05/91 18:55 New function. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JX  5/13/92 16:21 Remove unnecessary I, MORE. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"
#include "derr.h"

FUNCTION  cus_dusers (cnum, ctext, priorg, error)
   int    cnum;
   Chix   ctext, priorg;
   int   *error;
{

   ENTRY ("cus_dusers", "");

   /*** Open the XCUS userlist file for writing. */
   *error = DNOCONF;
   if (NOT unit_lock (XCUS, WRITE, L(cnum), L(0), L(0), L(0), nullchix))   RETURN(0);
   if (NOT unit_make (XCUS))         { unit_unlk (XCUS);   RETURN(0); }

   unit_write (XCUS, CQ(":primary "));
   unit_write (XCUS, priorg);
   unit_write (XCUS, CQ("\n"));
   unit_write (XCUS, ctext);

   unit_close (XCUS);
   unit_unlk  (XCUS);

   *error = 0;
   RETURN (1);
}
