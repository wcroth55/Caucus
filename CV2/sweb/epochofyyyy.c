
/*** EPOCH_OF_YYYY.   Return the epoch (time) value in seconds of SQL date.
/
/    when = epoch_of_yyyy (dstr);
/   
/    Parameters:
/       int4  when;       (returned time value in seconds since 1970)
/       Chix  dstr;       (date string, eg "1997-05-20 14:18")
/
/    Purpose:
/
/    How it works:
/
/    Returns:
/
/    Home:  sweb/epochofyyyy.c
/ */

/*** Copyright (C) 2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 06/09/06 New function, based loosely on epochof.c */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define LEAP(y) (y % 400 == 0) || ((y % 4 == 0)  &&  (y % 100 != 0))

FUNCTION  int4 epoch_of_yyyy (Chix dstr)
{
   static int days_in_month[12] =
      {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   char   date[100], word[100];
   int    day, year, month, hours, minutes, limit, total, mi, y, gmt, 
          now, debug;
   char  *strtoken();
   ENTRY ("epoch_of_yyyy", "");

   /*** Decode the day and year numbers. */
   ascofchx (date, dstr, 0, 99);
   if (strtoken (word, 1, date) == NULL)  RETURN (0);
   sscanf   (word,   "%4d", &year);
   sscanf   (word+5, "%2d", &month);      --month;
   sscanf   (word+8, "%2d", &day);

   if (strtoken (word, 2, date) == NULL)  RETURN (0);
   sscanf   (word,   "%2d", &hours);
   sscanf   (word+3, "%2d", &minutes);


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
