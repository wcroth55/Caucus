
/*** SYSYMD.   Convert time in seconds to year#, month#, etc.
/
/    sysymd (seconds, year, month, day, hour, minute, sec, wday);
/ 
/    Parameters:
/       int4       seconds;
/       int   *year;    (returned year #)
/       int   *month;   (returned month #, 1-12)
/       int   *day;     (returned day # in month, 1-31)
/       int   *hour;    (returned hour number, 0-24)
/       int   *minute;  (returned minute #, 0-59)
/       int   *sec;     (returned second #, 0-59)
/       int   *wday;    (returned week day #, 0-6 [sunday-sat] )
/
/    Purpose:
/       Convert an epoch time SECONDS (since 00:00 1 Jan 1970 GMT)
/       to year/month/day/hour/minute/second numbers, relative to
/       the current local time zone.
/
/    How it works:
/
/    Returns:
/
/    Known bugs:
/
/    Home:  sys2/sysymd.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/04/92 14:34 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR  6/06/97 23:00 Add doc. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#if SYSV | HPXA9 | BSD21 | LNX12 | NUT40 | WNT40 | FBS22
#include <time.h>
#endif

#if SUN41
#include <sys/time.h>
#endif

FUNCTION  sysymd (seconds, year, month, day, hour, minute, sec, wday)
   int4   seconds;
   int   *year, *month, *day, *hour, *minute, *sec, *wday;
{

#if UNIX | NUT40 | WNT40

   struct tm *now;
   time_t     secs;

   ENTRY ("sysymd", "");

   secs = seconds;
   now  = localtime (&secs);

   *year   = now->tm_year;
   if (*year < 1900)  *year += 1900;
   *month  = now->tm_mon+1;
   *day    = now->tm_mday;
   *hour   = now->tm_hour;
   *minute = now->tm_min;
   *sec    = now->tm_sec;
   *wday   = now->tm_wday;
#endif

   RETURN (1);
}
