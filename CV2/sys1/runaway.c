/*** RUN_AWAY_PROC.   Detect a run-away process.
/
/    exit = run_away_proc (bad);
/
/    Parameters:
/       int   exit;      (1 => probably run-away process, 0 => process ok)
/       int   bad;       (1 => could be run-away, 0 => process ok)
/
/    Purpose:
/       Try to detect a run-away process, i.e. one that is going around
/       infinitely in a tight loop, such as a failed-read loop.
/
/    How it works:
/       If more than MAXLOOPS calls with BAD==1 occur within a period
/       of one second, this is considered a run-away process.
/
/    Returns:
/       1 process appears to be running away, caller should exit
/       0 process appears ok
/
/    Error Conditions:  none
/ 
/    Related functions:
/
/    Home:  sys1/runaway.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/19/95 13:25 New function. */

#include <stdio.h>
#include "handicap.h"
#include "caucus.h"

#include "systype.h"
#include "sys.h"

#define  MAXLOOPS   100

FUNCTION  run_away_proc (bad)
   int    bad;
{
   static int  loops = 0;
   static int4 when  = L(0);
   int4   systime();

   if (bad   == 0)  { loops = 0;   return(0); }

   if (loops == 0)    when  = systime();

   if (++loops > MAXLOOPS) {
      if ((systime() - when) <= 1)  return (1);
      loops = 0;
   }
   return (0);
}
