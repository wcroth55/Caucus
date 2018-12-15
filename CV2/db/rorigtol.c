
/*** RORIG_TO_LOCAL.  Convert original response number to local response number.
/
/    ok = rorig_to_local (cnum, localinum, origrnum, respchn, &local);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    cnum;        (conference number)
/       int    localinum;   (local item number of item containing response)
/       int    origrnum;    (response number on original host)
/       Chix   respchn;     (name of original host)
/       int   *local;       (response number on local host)
/
/    Purpose:
/       When CaucusNet gets a transaction from another host that pertains to
/       a response, the transaction contains the response number on the 
/       "original" host where the response was first entered, and the name
/       of that host.  In order to apply a transaction to the local
/       host, we must be able to map the original response number into a
/       response number on the local host.
/
/    How it works:
/       The original response number and host name for each response is
/       kept in the text of the partfile for that response.  Rorig_to_local()
/       calls getline2() which calls loadtext(), which loads the response
/       number and host name into the global "block" structure, where we
/       access it directly.
/
/       This means, alas, that rorig_to_local() may have to scan every
/       single response in a particular item in order to find the mapping.
/
/    Returns: 1 on success
/             0 on no such response or error
/
/    Error Conditions:
/  
/    Side effects: may reload globals master, confnum.
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/rorigtol.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/25/91 14:52 New function. */
/*: CR  7/23/91 23:29 Check RESPCHN against this host. */
/*: CR  7/26/91 18:21 Correct chxofascii() call. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: JX  5/13/92 14:45 Map BLOCK to TBUF, chixify. */

#include <stdio.h>
#include "caucus.h"

extern struct master_template  master;
extern Textbuf                 tbuf;
extern short                   confnum;

FUNCTION  rorig_to_local (cnum, localinum, origrnum, respchn, local)
   int    cnum, localinum, origrnum, *local;
   Chix   respchn;
{
   int    r, b, success;
   Chix   thishost;
   Chix   getline2();

   ENTRY ("rorig_to_local", "");

   thishost    = chxalloc (L(80), THIN, "rorigtol thishost");
   success     = 0;

   /*** An empty respchn means this host. */
   getmyhost (thishost);
   if (EMPTYCHX (respchn)  ||  chxeq (respchn, thishost)) 
      { *local = origrnum;   SUCCEED; }

   /*** Make sure we've loaded the XCMA file for the proper conference
   /    number. */
   if (master.cnum != cnum)
      if (NOT srvldmas (1, &master, cnum))  FAIL;

   if (localinum > master.items  ||  DELETED (localinum))  FAIL;

   confnum = cnum;
   for (r=1;   r<=master.responses[localinum];   ++r) {
      if (getline2 (XCRD, XCRF, cnum, nullchix, localinum, r, 0, (Textbuf) NULL)
          == nullchix) continue;
      b = r - tbuf->r_first;
#if C_LINK
      if (tbuf->origresp[b] == origrnum  &&
          tbuf->orighost[b] != nullchix  &&
          chxeq (tbuf->orighost[b], respchn))
         {  *local = r;    SUCCEED; }
#endif
   }
   FAIL;

 done:
   chxfree ( thishost );

   RETURN ( success );
}
