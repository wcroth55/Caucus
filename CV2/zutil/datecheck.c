
/*** DATECHECK.   Is a given date/time within LIMIT seconds of now?
/
/    Invoked as:
/       datecheck   date  time  limit  gmt  [-d]
/
/    Arguments:
/       date       a date string, such as "22-MAY-97"
/       time       a time string, such as "13:20"
/       limit      number of seconds
/       gmt        GMT correction, e.g "-5"
/       [-d]       optional debugging argument.
/
/    Purpose:
/       Determines if the DATE & TIME string is within LIMIT seconds
/       of "now".
/
/    How it works:
/       Writes "1" to stdout if (DATE TIME) > now - LIMIT, i.e if
/       the DATE & TIME occurred within the last LIMIT seconds.
/
/       Writes "0" otherwise, or if the argument syntax was wrong.
/
/       The GMT argument corrects for systems that display time in
/       one manner, but actually record it in another.  E.g. for
/       many systems display EST the GMT value "-5" properly corrects
/       the results of datecheck.
/
/       To determine the proper value of GMT, use the debugging flag as
/       well to see what results DATECHECK is producing.
/
/    Exit status: 0
/
/    Known bugs:
/
/    Home:  zutil/datecheck.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/30/97 13:50 New program, based on api/timeof.c */

#include <stdio.h>

#define LEAP(y) (y % 400 == 0) || ((y % 4 == 0)  &&  (y % 100 != 0))

main (int argc, char* argv[])
{
   static int days_in_month[12] =
      {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   static char *usa_months[] =
      {"jan", "feb", "mar", "apr", "may", "jun", 
       "jul", "aug", "sep", "oct", "nov", "dec"};
   int    day, year, month, hours, minutes, limit, total, mi, y, gmt, 
          now, debug;
   char   monthname[80], *m;


   if (argc < 4) {
      printf ("0\n");
      exit   (0);
   }

   /*** Decode the day and year numbers. */
   sscanf (argv[1],   "%2d", &day);
   sscanf (argv[1]+7, "%2d", &year);
   if (year < 70)  year += 100;
   year += 1900;

   /*** Decode the TIME and the LIMIT. */
   sscanf (argv[2], "%2d:%2d", &hours, &minutes);
   sscanf (argv[3], "%d", &limit);

   /*** Decode GMT if it exists. */
   gmt = 0;
   if (argc > 4)  sscanf (argv[4], "%d", &gmt);

   /*** Decide if debugging mode. */
   debug = (argc == 6);

   /*** Decode the month number. */
   strcpy (monthname, argv[1]+3);
   monthname[3] = '\0';
   for (m=monthname;   *m;   ++m)
      if (*m >= 'A'  &&  *m <= 'Z')  *m = (*m - 'A') + 'a';

   for (month=0;   month<12;   ++month) 
      if (strcmp (monthname, usa_months[month]) == 0)  break;

   if (month >= 12) {
      printf ("0\n");
      exit   (1);
   }


   /*** Total up the number of seconds since the dawn of time, assumed
   /    to be Jan 1, 1970. */
   for (total=0, y=1970;   y<year;   ++y) 
      total += (LEAP(y) ? 366 : 365);

   days_in_month[1] = LEAP(year) ? 29 : 28;

   for (mi=0;   mi<month;   ++mi)   total += days_in_month[mi];
   total += day - 1;
   total  = total * 24L * 3600L;
   total  = total + 3600 * hours + 60 * minutes;
   now    = time(NULL) + 3600 * gmt;

   if (debug) {
      printf ("now  = %11d\n", now);
      printf ("date = %11d\n", total);
      printf ("delta= %11d\n", now - total);
      printf ("gmt  = %11d\n", gmt);
   }
   if (total > now - limit)  printf ("1\n");
   else                      printf ("0\n");
   exit (0);
}
