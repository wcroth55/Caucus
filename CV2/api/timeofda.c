/*** TIME_OF_DATE.   Convert a date string to absolute time in seconds.
/
/    seconds = time_of_date (datearg, months);
/
/    Parameters:
/       int4   seconds;     (resulting number of seconds)
/       Chix   datearg;     (date in string form)
/       Chix   months;      (list of names of months)
/
/    Purpose:
/       Convert a date string to its equivalent absolute time in seconds.
/
/    How it works:
/       Handles the following date forms:
/          dd-mmm-yy   (e.g., 1-FEB-93)
/          dd-mmm      (e.g., 23-MAY, assumes current year)
/          mm/dd/yy    (e.g., 2/1/93)
/          mm/dd       (e.g., 5/23,   assumes current year)
/
/       MONTHS must be a list of the names of the months (as in the
/       'mmm' examples), separated by blanks.  If it is nullchix, 
/       time_of_date() will use:
/          "jan feb mar apr may jun jul aug sep oct nov dec".
/
/    Returns: equivalent absolute time in seconds
/             L(0) on error
/
/    Error Conditions:
/       null  datearg
/       empty datearg
/       uninterpretable format in datearg
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/       Uses systime() and sysymd() for system dependent stuff.
/
/    Known bugs:      none
/
/    Home:  api/timeofda.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/24/93 23:41 New function. */
/*: JV  8/24/93 16:44 Robustify and add warning about CQ(). */
/*: CN  5/15/93 22:44 Expand ok date syntax, / and - are equivalent. */
/*: CP  8/25/93 14:08 Integrate, fix month sanity check, remove CQ use. */
/*: CB  9/17/93 14:51 Initialize 'dash' and 'slash'! */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "done.h"

static int days_in_month[12] =
   {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static char usa_months[] =
   {"jan feb mar apr may jun jul aug sep oct nov dec"};

FUNCTION  int4  time_of_date (Chix datearg, Chix months)
{
   int4   total, zero, now;
   int    month, day, year, y, m, i, wday;
   int    tmonth, tday, thour, tminute, tsec;
   Chix   monthlist, strday, temp, strmon, stryear, datestr;
   Chix   slash, dash;
   int4   success, middle, pos;
   int4   systime();

   year  = -1;
   month = day = 0;

   /*** Make a temp copy of DATEARG, and replace '-'s with '/'s. */
   datestr = chxalloc (L(40), THIN, "time_of datestr");
   slash   = chxalloc ( L(8), THIN, "time_of slash");
   dash    = chxalloc ( L(8), THIN, "time_of dash");
   chxcpy (datestr, datearg);
   chxofascii (slash, "/");
   chxofascii (dash,  "-");
   while  (chxalter (datestr, L(0), dash, slash)) ;

   /*** Is this 'mm/dd/yy' or 'dd/mmm/yy'?  Look after the first
   /    '/' and see if it is a number. */
   if ( (middle = chxindex (datestr, L(0), slash)) < 0)  DONE(L(0));
   pos = middle + 1;
   chxint4 (datestr, &pos);

   /*** Handle 'mm/dd/yy' date forms. */
   if (pos > middle + 1) {
      sscanf (ascquick(datestr), "%d/%d/%d", &month, &day, &year);
   }

   /*** Handle the (more complicated) 'dd-mmm-yy' forms. */
   else if (chxindex (datestr, L(0), slash) >= 0) {
      monthlist = chxalloc (L(60), THIN, "time_of monthlist");
      strday    = chxalloc (L(20), THIN, "time_of strday");
      temp      = chxalloc (L(20), THIN, "time_of temp");
      strmon    = chxalloc (L(20), THIN, "time_of strmon");
      stryear   = chxalloc (L(20), THIN, "time_of stryear");

      /*** Get the list of months. */
      if (months==nullchix)  chxofascii (monthlist, usa_months);
      else                   chxcpy     (monthlist, months);

      /*** Pluck out the string forms of the day, month, and year. */
      chxbreak  (datestr, strday, temp,    slash);
      chxbreak  (temp,    strmon, stryear, slash);
      jixreduce (strmon);

      /*** Decode the day and the year. */
      zero = L(0);
      day  = chxint4 (strday, &zero);
      zero = L(0);
      if (NOT EMPTYCHX(stryear))  year = chxint4 (stryear, &zero);

      /*** Decode the month. */
      for (month=(-1), i=1;   i<=12;   ++i) {
         chxtoken  (temp, nullchix, i, monthlist);
         if (chxeq (temp, strmon))  { month=i;   break; }
      }

      chxfree (monthlist);
      chxfree (strday);
      chxfree (strmon);
      chxfree (stryear);
      chxfree (temp);
   }

   else DONE (L(0));

   /*** Make sure we got a valid month out of the previous methods. */
   if (month <= 0  ||  month > 12)  DONE (L(0));

   /*** If we didn't get an explicit year, use the current one. */
   if (year < 0) {
      now = systime();
      sysymd (now, &year, &tmonth, &tday, &thour, &tminute, &tsec, &wday);
   }
   if (year > 1900)  year = year - 1900;

   /*** Total up the number of seconds since the dawn of time, assumed
   /    to be Jan 1, 1970. */
   total = 0;
   for (y=70;   y<year;   ++y) {
      if ( (1900 + y) % 4 == 0)  total += 366;
      else                       total += 365;
   }
   if ( (1900 + year) % 4 == 0)  days_in_month[1] = 29;
   else                          days_in_month[1] = 28;
   for (m=1;   m<month;   ++m)   total += days_in_month[m-1];
   total += day - 1;
   total = total * L(24) * L(3600);
   DONE(total);

done:
   chxfree (datestr);
   chxfree (slash);
   chxfree (dash);
   return  (success);
}
