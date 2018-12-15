/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** sysmsec(mode)   Returns elapsed time in milliseconds.
/
/        sysmsec(0);     Starts the timer.
/    n = sysmsec(1);     Returns # of milliseconds since last sysmsec(0) call.
/
/ */

#include <sys/time.h>

int sysmsec (int mode) {
   static int      s0, m0;
   struct timeval  t;
   struct timezone z;
   int             msec;

   gettimeofday (&t, &z);
   if (mode == 0) {
      s0 = t.tv_sec;
      m0 = t.tv_usec;
      return (0);
   }

   if (m0 > t.tv_usec) {
      t.tv_usec += 1000000;
      t.tv_sec  -= 1;
   }
   msec = (t.tv_sec - s0) * 1000 + (t.tv_usec - m0) / 1000;

   return (msec);
}
