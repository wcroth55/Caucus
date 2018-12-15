
/*** SYSTIMEZONE().   Return timezone as delta seconds from GMT.
/
/    delta = systimezone();
/ 
/    Parameters:
/       int4   delta;   (returned # of seconds)
/
/    Purpose:
/       Return the local timezone as the difference in seconds 
/       between local time and GMT.  (For example, in an EST
/       zone, returns -18000.)
/
/    How it works:
/       For many unixes, calling localtime() sets the external 
/       variable 'timezone' with this information.  Others 
/       supply a member in the 'tm' structure.  The differences
/       are subtle, and should be checked carefully.
/
/    Home:  sys2/systimez.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/06/92 23:30 New function. */

#include <stdio.h>
#include "systype.h"

#if WNT40
#include <sys/timeb.h>
#include <time.h>
#endif

#if AIX41 | BSD21 | DEC32 | FBS22 | HPXA9 | LNX12 | SOL24 | SOL25 | FBS22
#include <time.h>
#endif

#if SUN41
#include <sys/time.h>
extern long timezone;
#endif

#if IRX53 | SOL24 | SOL25
#include <time.h>
extern time_t timezone, altzone;
#endif

#define  FUNCTION

FUNCTION  systimezone()
{
#if UNIX
   static int myzone = -1;
   struct tm *now;
   time_t     secs;

   if (myzone != -1)   return (myzone);

   secs = time(NULL);
   now  = localtime (&secs);

#if BSD21 | DEC32 | FBS22
   myzone = now->tm_gmtoff;
#endif

#if LNX12 | SUN41 | HPXA9 | AIX41
   myzone = -timezone;
   if (now->tm_isdst > 0)  myzone = myzone + 3600;
#endif

#if IRX53 | SOL24 | SOL25
   if (now->tm_isdst > 0)  myzone = -altzone;
   else                    myzone = -timezone;
#endif

   return (myzone); 
#endif

#if WNT40
   struct _timeb timebuffer;

   _ftime( &timebuffer );
   return (-60 * timebuffer.timezone + (timebuffer.dstflag ? 3600 : 0));
#endif


}
