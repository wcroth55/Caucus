
/*** IORIG_TO_LOCAL.  Convert original item number to local item number.
/
/    ok = iorig_to_local (cnum, originum, itemchn, &local);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    cnum;        (conference number)
/       int    originum;    (item number on original host)
/       char  *itemchn;     (name of original host)
/       int   *local;       (item number on local host)
/
/    Purpose:
/       When CaucusLink gets a transaction from another host that pertains to
/       an item, the transaction contains the item number on the "original"
/       host for that item.  In order to apply a transaction to the local
/       host, we must be able to map the original item number into an item
/       number on the local host.
/
/    How it works:
/       The mapping between local and original item number is kept in each
/       host's XCMA file for a given conference.  Many times that data is
/       already loaded in the global "master" structure.  If it is loaded,
/       iorig_to_local() just uses what's there; otherwise, it loads the
/       XCMA file into master.
/
/
/    Returns: 1 if the mapping succeeded and the item exists.
/             0 if the mapping failed (local set to 0)
/             0 if the mapping succeeded, but the item does not exist
/               (local set to the item number).
/
/    Error Conditions:
/  
/    Side effects: may reload "master" structure.
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/iorigtol.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/25/91 14:52 New function. */
/*: CR  7/23/91 23:27 Check ITEMCHN against this host. */
/*: CR  7/26/91 18:22 Correct chxofascii() call. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR  1/14/92 12:43 Return 0 if mapping succeeds, but item was deleted. */
/*: JX  5/21/92 14:22 Chixify. */
/*: JV 11/17/92 17:50 Change orighost from Chix to short. */

#include <stdio.h>
#include "caucus.h"

extern struct master_template  master;
extern Chix *hostnames;

FUNCTION  iorig_to_local (cnum, originum, itemchn, local)
   int    cnum, originum, *local;
   Chix   itemchn;
{
   Chix   chn;
   int    i, success;
   Chix   thishost;

   ENTRY ("iorig_to_local", "");

   thishost = chxalloc (L(40), THIN, "iorig thishost");

   /*** Make sure we've loaded the XCMA file for the proper conference
   /    number. */
   if (master.cnum != cnum)
      if (NOT srvldmas (1, &master, cnum))  FAIL;

   /*** An empty itemchn means this host. */
   getmyhost (thishost);
   if (EMPTYCHX (itemchn)  ||  chxeq (itemchn, thishost)) 
      { *local = originum;   SUCCEED; }

   chn = chxalloc (L(20), THIN, "iorig_to_local chn");
   chxcpy (chn, itemchn);

   for (i=1;   i<=master.items;   ++i)
      if (master.orighost[i] > 0   &&
          chxeq (hostnames[master.orighost[i]], chn)  &&
          master.origitem[i] == originum)  break;

   chxfree (chn);
   if (i > master.items) { *local = 0;   FAIL; }

   *local = i;
   if (master.responses[i] >= 0) SUCCEED
   else                          FAIL;

done:
   chxfree (thishost);
   RETURN (success);
}
