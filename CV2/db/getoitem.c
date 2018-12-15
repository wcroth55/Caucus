
/*** GETOITEM.   Get original host and item # for a given local item.
/
/    ok = getoitem (origitem, orighost, item, master);
/
/    Parameters:
/       int   ok;                         (success?)
/       int  *origitem;                   (place to put original item number)
/       Chix  orighost;                   (name of original host)
/       int   item;                       (local item number)
/       struct master_template *master;   (XCMA in memory for this conf.)
/
/    Purpose:
/       Given a local item number, get the name of the original host on
/       which this item was entered, and the number of the item on that host.
/       
/    How it works:
/       Getoitem() looks up the information about ITEM in MASTER, and
/       puts it in a usable form in ORIGITEM and ORIGHOST.
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/getoitem.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/19/91 14:26 New function. */
/*: JX  5/19/92 12:37 Finish chixification, carve. */
/*: JV 11/17/92 17:48 Change orighost from Chix to short. */

#include <stdio.h>
#include "caucus.h"

extern Chix *hostnames;

FUNCTION  getoitem (origitem, orighost, item, master)
   int   *origitem;
   Chix   orighost;
   int    item;
   struct master_template *master;
{

   ENTRY ("getoitem", "");

   if (master->orighost[item] > 0) {
      chxcpy (orighost, hostnames[master->orighost[item]]);
      *origitem = master->origitem[item];
   }
   else {
      getmyhost (orighost);
      *origitem = item;
   }

   RETURN (1);
}
