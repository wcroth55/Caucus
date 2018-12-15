
/*** LOGFEATURE.   Log use of a "feature" in XULG logging file.
/
/    logfeature (feature, value, starttime, count, size);
/
/    Parameters:
/       Chix    feature;     (name of feature)
/       Chix    value;       (value, e.g. conf name)
/       int4    ivalue;      (numeric value, e.g. item number)
/       int4    starttime;   (time use of feature started)
/       int4    count;       (number of uses of feature)
/       int4    size;        (size of 'thing' used or did)
/       
/    Purpose:
/       Write a record to the current user's XULG feature logging file
/       recording the use of a particular feature.
/
/    How it works:
/       Note: FEATURE may be of the form CQ("xxxx").  (This is unusual,
/       but desireable in this case.)
/
/    Returns:
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/logfeat.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/04/92 16:20 New function. */
/*: CR  9/09/92 14:20 Add ivalue arg.  */
/*: CI 10/07/92 17:24 Chixify. */
/*: CR 10/13/92 14:55 Remove extra \n from NUMBERS. */

#include <stdio.h>
#include "caucus.h"

extern struct flag_template flag;

FUNCTION  logfeature (feature, value, ivalue, starttime, count, size)
   Chix   feature, value;
   int4   ivalue, starttime, count, size;
{
   Chix   userid, temp, fcopy;
   char   numbers[100];
   int    year, month, day, hour, min, sec, success, wday;
   int4   now, duration, systime();

   ENTRY  ("logfeature", "");

   if (NOT flag.logfeature)  RETURN (1);

   userid = chxalloc (L(20), THIN, "logfeat userid");
   temp   = chxalloc (L(90), THIN, "logfeat temp");
   fcopy  = chxalloc (L(20), THIN, "logfeat fcopy");
   chxcpy (fcopy, feature);

   sysuserid (userid);
   if (NOT unit_lock   (XULG, WRITE, L(0), L(0), L(0), L(0), userid))  FAIL;
   if (NOT unit_append (XULG))           { unit_unlk (XULG);   FAIL; }

   now = systime();
   duration = (starttime > L(0) ? now - starttime : L(0));
   if (duration > L(48) * L(3600))  duration = L(0);
   sysymd  (now, &year, &month, &day, &hour, &min, &sec, &wday);

   sprintf (numbers, "%5d %02d%02d%02d %02d%02d%02d %6d %3d %7d",
                     ivalue, year, month, day, hour, min, sec,
                     duration, count, size);
   chxformat  (temp, CQ("%-12s %-8s "), L(0), L(0), userid, fcopy);
   chxformat  (temp, CQ("%-12s %s\n"),  L(0), L(0), value, chxquick(numbers,1));
   unit_write (XULG, temp);
   unit_close (XULG);
   unit_unlk  (XULG);
   SUCCEED;

done:
   chxfree (userid);
   chxfree (temp);
   chxfree (fcopy);
   RETURN  (success);
}
