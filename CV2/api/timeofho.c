
/*** TIME_OF_HOUR.   Convert an hour/minute string to absolute time in seconds.
/
/    seconds = time_of_hour (timestr);
/
/    Parameters:
/       int4   seconds;     (resulting number of seconds)
/       Chix   timestr;     (hour in string form, e.g. "hh:mm:ss")
/
/    Purpose:
/       Convert an hour/minute string to its equivalent time in seconds.
/
/    How it works:
/
/    Returns: equivalent time in seconds (e.g. "23:59" returns L(86340))
/             0 on error
/
/    Error Conditions:
/       null  timestr
/       empty timestr
/       uninterpretable format in timestr
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
/    Home:  api/timeofho.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  5/10/93 23:02 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  int4  time_of_hour (Chix timestr)
{
   int4   pos, hour;

   pos   = L(0);
   hour  = chxint4 (timestr, &pos) * L(3600);

   if (chxvalue (timestr, pos) != ':')  return (hour);
   ++pos;
   hour += chxint4 (timestr, &pos) *   L(60);

   if (chxvalue (timestr, pos) != ':')  return (hour);
   ++pos;
   hour += chxint4 (timestr, &pos);

   return (hour);
}
