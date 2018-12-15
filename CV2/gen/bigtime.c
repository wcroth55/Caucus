/*** BIGTIME    Manipulate time in seconds and microseconds.
/
/    bigtime     (bt);
/    bigtime_add (bt, bta, btb);
/    bigtime_sub (bt, bta, btb);
/   
/    Parameters:
/       int  bt[2], bta[2], btb[2];
/
/    Purpose:
/       bigtime(bt) sets bt[0] to the epoch time in seconds,
/                        bt[1] to the time since bt[0] in microseconds
/
/       bigtime_add effectively sets bt = bta + btb
/       bigtime_sub effectively sets bt = bta - btb
/
/    How it works:
/
/    Home: bigtime.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/01/03 New function. */

#include <stdio.h>
#include <sys/time.h>

#define  FUNCTION

FUNCTION void bigtime (int *bt) {
   struct timeval  tv;
   struct timezone tz;

   gettimeofday (&tv, &tz);
   bt[0] = tv.tv_sec;
   bt[1] = tv.tv_usec;
   return;
}

FUNCTION void bigtime_add (int *bt, int *bta, int *btb) {
   int   sec, mic, delta=0;

   mic = bta[1] + btb[1];
   if (mic > 1000000) { mic -= 1000000;  delta = 1; }
   sec = bta[0] + btb[0] + delta;
   bt[0] = sec;
   bt[1] = mic;
}

FUNCTION void bigtime_sub (int *bt, int *bta, int *btb) {
   int   sec, mic, delta=0;

   mic = bta[1] - btb[1];
   if (mic < 0) { mic += 1000000;   delta = -1; }
   sec = bta[0] - btb[0] + delta;
   bt[0] = sec;
   bt[1] = mic;
}
