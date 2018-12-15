
/*** SYSDAYTIME.   Turn a number of seconds into a date and a time.
/
/    ok = sysdaytime (day, time, format, seconds);
/
/    Parameters:
/       int   ok;       // Success?
/       int   format;   // input:  format code, see below:
/       Chix  day;      // output: date, in format FORMAT
/       Chix  time;     // output: time, in format FORMAT
/       int4  seconds;  // input:  time in seconds since epoch.
/
/    Purpose:
/       Convert time (in seconds since epoch) into chix date and time.
/
/    How it works:
/       Simple character manipulation.
/       FORMAT    DAY             TIME
/       ------    ---             ----
/         0       dd-MMM-yy       hh:mm     (no dictionary access)
/         1       dd-MMM-yy       hh:mm     (uses dictionary for month names)
/         2       Month dd, yyyy  hh:mm
/         3       dd Month, yyyy  hh:mm
/         4       yy/mm/dd        hh:mm
/
/       If FORMAT is not in the table, it assumes zero.
/       
/    Returns: 1
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions: sysymd()
/
/    Called by:  Application.
/
/    Operating system dependencies: 
/
/    Known bugs:  FORMATS other than zero don't work yet.
/
/    Home:  sys/sysdayti.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  6/05/92 23:03 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR 10/12/05 Remove mdstr() call for C5. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "systype.h"

FUNCTION  sysdaytime (day, time, format, seconds)
   Chix day, time;
   int  format;
   int4 seconds;
{
   static Chix s_months;
   Chix   mon, months;
   int    iyear, imonth, iday, ihour, iminute, isec, wday;

   ENTRY ("sysdaytime", "");

   mon    = chxalloc (  L(8), THIN, "sysday mon");
   months = chxalloc (L(100), THIN, "sysday months");

   if (format==0)
      chxofascii (months, "JAN FEB MAR APR MAY JUN JUL AUG SEP OCT NOV DEC");
 
   sysymd   (seconds, &iyear, &imonth, &iday, &ihour, &iminute, &isec, &wday);
   chxtoken (mon, nullchix, imonth, months);
   chxclear (day);
   chxclear (time);

   switch (format) {
      case 0:
      default:
          chxformat (day,  CQ("%02d-%s-%4d"), L(iday), L(iyear), mon, nullchix);

          chxformat (time, CQ("%02d:%02d"), L(ihour), L(iminute),
                     nullchix, nullchix);
          break;
   }

   chxfree (mon);
   chxfree (months);

   RETURN  (1);
}
