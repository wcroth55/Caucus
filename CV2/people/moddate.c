
/*** MODDATE.   Modify a datemap namelist file entry.
/
/    moddate (action, ud, uf, cnum, item, date, time, value);
/
/    Parameters:
/       int       n;         (success?)
/       int       action;    (1=add entry, 0=delete entry)
/       int       ud, uf;    (namelist file unit numbers)
/       int       cnum;      (conference number)
/       int4      item;      (item number, if relevant)
/       Chix      date;      (string form of date)
/       Chix      time;      (string form of time)
/       int4      value;     ("value" part of namelist entry)
/
/    Purpose:
/       Add or delete an entry from a datemap namelist file.
/
/    How it works:
/
/    Returns: 1 on success
/             0 on error (modnames fails)
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  chgdresp(), chgditem()
/
/    Operating system dependencies: None.
/
/    Known bugs:
/
/    Home:  people/moddate.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  5/10/93 23:35 New function. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  moddate (action, ud, uf, cnum, item, date, time, value)
   int    action, ud, uf, cnum;
   int4   item;
   Chix   date;
   Chix   time;
   int4   value;
{
   Namelist add, del, nlnode(), nlnames();
   Chix     abstime, entryval;
   char     temp[100];
   int4     time_of_date(), time_of_hour();
   int      success;

   abstime  = chxalloc (L(20), THIN, "moddate abstime");
   entryval = chxalloc (L(20), THIN, "moddate entryval");

   sprintf (temp, "%010d", time_of_date (date, nullchix) +
                            time_of_hour (time));
   chxcpy  (abstime, CQ(temp));

   sprintf (temp, "%04d", value);
   chxcpy  (entryval,  CQ(temp));

   if (action) { add = nlnames (abstime, entryval);    del = nlnode (1); }
   else        { del = nlnames (abstime, entryval);    add = nlnode (1); }

   success = modnames (ud, uf, nullchix, cnum, item, add, del, NOPUNCT);

   nlfree  (add);
   nlfree  (del);
   chxfree (abstime);
   chxfree (entryval);

   return  (success);
}
