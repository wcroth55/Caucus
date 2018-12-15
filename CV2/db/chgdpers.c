/*** CHG_DPERSON.  Add or change a remote person in the local people database.
/
/    ok = chg_dperson (newname, oldname, id, host, phone, error);
/
/    Parameters:
/       int    ok;          (Success?)
/       Chix   newname;     (remote person's new name)
/       Chix   oldname;     (remote person's old name)
/       Chix   id;          (remote person's userid)
/       Chix   host;        (remote person's home host)
/       Chix   phone;       (remote person's phone number)
/       int   *error;       (return error code)
/
/    Purpose:
/       Chg_dperson() provides a way for the XFU to add or change information
/       about a remote person to the local database.
/
/       This version of chg_dresp() adds or changes the remote person's
/       name in the local XSNN namepart files.  It ignores the PHONE
/       information.
/
/    How it works:
/       Chg_dperson() removes any XSNN entries for OLDNAME with this ID
/       and HOST.  It then adds XSNN entries for NEWNAME.
/
/    Returns: 1 on success, sets ERROR to 0.
/             0 on any error condition, see below.
/
/    Error Conditions: sets value of ERROR accordingly:
/       DBADDB    could not lock/open XSND/XSNN namepart files
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
/    Home:  db/chgdpers.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/17/91 11:45 New function. */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: JX  5/26/92 13:51 Chix mark. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */

#include <stdio.h>
#include "caucus.h"
#include "derr.h"

FUNCTION  chg_dperson (newname, oldname, id, host, phone, error)
   Chix  newname, oldname, id, host, phone;
   int   *error;
{
   Chix     idhost;
   Namelist add, del, nlnames();
   int      ok;

   ENTRY ("chg_dperson", "");

   idhost    = chxalloc (L(100), THIN, "chgdpers idhost");

   *error = NOERR;

   chxconc (idhost, id, CQ("@"));
   chxcat  (idhost, host);

   /*** Replace the user's old name with the new name.  Create a list of
   /    nameparts to DELete, and a list of nameparts to ADD.  Hand them
   /    off to MODNAMES to do the dirty work. */
   add = nlnames (newname, idhost);
   del = nlnames (oldname, idhost);
   ok = modnames (XSND, XSNN, nullchix, 0, L(0), add, del, NOPUNCT);
   nlfree (add);
   nlfree (del);

   if (NOT ok)  *error = DBADDB;

   chxfree (idhost);

   RETURN (ok);
}
