/*** CUS_DPORG.   Customize primary organizer of a conference.
/
/    ok = cus_dporg (cnum, newid, master, error);
/
/    Parameters:
/       int    ok;                        (Success?)
/       int    cnum;                      (conference number)
/       Chix   newid;                     (userid@host of new organizer)
/       struct master_template *master;   (a "master" item/resp record)
/       int   *error;                     (return error code)
/
/    Purpose:
/       Cus_dporg() is the one true way of changing the primary
/       organizer of a conference.  All Caucus and CaucusLink code
/       that change the primary organizer must call this function.
/
/    How it works:
/       The organizer "id" for conference CNUM is set to NEWID.
/       If NEWID does not contain an "@host", the current host
/       is used.
/
/       Cus_dporg() loads the "master" item/resp record for conference
/       CNUM into MASTER.
/
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
/    Called by:  Caucus functions, XFU
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/cusdporg.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/05/91 22:58 New function. */
/*: CR 11/14/91 11:49 Pass master as arg to store_master() call. */
/*: CR 11/19/91 17:52 If no @ in NEWID, use current host. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: DE  5/26/92 11:22 Fix a chxcpy */
/*: CL 12/10/92 15:22 Long chxindex args. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"
#include "derr.h"

FUNCTION  cus_dporg (cnum, newid, master, error)
   int    cnum;
   Chix   newid;
   struct master_template *master;
   int   *error;
{
   Chix   host, priorg;
   Chix   ulist;
   int    success, errors;

   ENTRY ("cus_dporg", "");

   host     = chxalloc (L(160), THIN, "cusdporg host");
   priorg   = chxalloc ( L(40), THIN, "cusdporg priorg");
   success  = 0;

   *error = DNOCONF;
   if (NOT unit_lock (XCMA, WRITE, L(cnum), L(0), L(0), L(0), nullchix))  FAIL;

   if (NOT srvldmas (0, master, cnum))  { unit_unlk (XCMA);   FAIL; }

   chxcpy (master->organizerid, newid);
   if (chxindex (newid, L(0), CQ("@")) < 0) {
      chxcat    (master->organizerid, CQ("@"));
      getmyhost (host);
      chxcat    (master->organizerid, host);
   }
   store_master (master);
   unit_unlk (XCMA);

   ulist = chxalloc (L(400), THIN, "cusdporg ulist");
   load_ulist  (XCUS, cnum, ulist, priorg, &errors);
   store_ulist (cnum, XCUS, ulist, newid);
   chxfree (ulist);

   *error = NOERR;
   SUCCEED;

 done:
   chxfree (host);
   chxfree (priorg);

   RETURN  (success);
}
