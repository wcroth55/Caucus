
/*** EPOCH_OF.   Return the epoch (time) value in seconds of a date string.
/
/    when = epoch_of (dstr);
/   
/    Parameters:
/       int4  when;       (returned time value in seconds since 1970)
/       Chix  dstr;       (date string, eg "20-MAY-97 14:18")
/
/    Purpose:
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/epochof.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/27/97 15:28 New function. */
/*: CR  6/07/98 18:19 Simplify, use systimezone(). */
/*: CR 02/07/05 Better bad detection! */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define LEAP(y) (y % 400 == 0) || ((y % 4 == 0)  &&  (y % 100 != 0))

FUNCTION  int4 epoch_of (Chix dstr)
{
   static int days_in_month[12] =
      {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   static char *usa_months[] =
      {"jan", "feb", "mar", "apr", "may", "jun", 
       "jul", "aug", "sep", "oct", "nov", "dec"};
   char   date[100];
   char  *hptr;
   int    day, year, month, hours, minutes, limit, total, mi, y, gmt, 
          now, debug;
   char   monthname[80], *m;
   ENTRY ("epoch_of", "");

   /*** Decode the day and year numbers. */
   strcpy (date, ascquick(dstr));
   if (strlen (date) < 8)       RETURN (0);
   if (NOT cf_digit (date[0]))  RETURN (0);
   if (date[1] == '-')  strconc (date, "0", date);
   sscanf (date,   "%2d", &day);
   sscanf (date+7, "%d",  &year);
   if (year <   70)  year +=  100;
   if (year < 1900)  year += 1900;

   /*** Decode the TIME and the LIMIT. */
   hptr = date;
   for (hptr=date;  *hptr && *hptr!=' ';   ++hptr) ;
   hours = minutes = 0;
   if (*hptr)  sscanf (hptr, "%d:%2d", &hours, &minutes);

   /*** Decode the month number. */
   strcpy (monthname, date+3);
   monthname[3] = '\0';
   for (m=monthname;   *m;   ++m)
      if (*m >= 'A'  &&  *m <= 'Z')  *m = (*m - 'A') + 'a';

   for (month=0;   month<12;   ++month) 
      if (strcmp (monthname, usa_months[month]) == 0)  break;

   /*** Total up the number of seconds since the dawn of time, assumed
   /    to be Jan 1, 1970. */
   for (total=0, y=1970;   y<year;   ++y) 
      total += (LEAP(y) ? 366 : 365);

   days_in_month[1] = LEAP(year) ? 29 : 28;

   for (mi=0;   mi<month;   ++mi)   total += days_in_month[mi];
   total += day - 1;
   total  = total * L(24) * L(3600);
   total  = total + 3600 * hours + 60 * minutes;
   total  = total - systimezone(); 

   RETURN (total);
}
