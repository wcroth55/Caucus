
/*** SYSAINIT.   System-dependent code required for API initialization.
/
/    sysainit (phase);
/
/    Parameters:
/       int       phase;  (0=>init; 1=>terminate)
/
/    Purpose:
/       Sysainit() contains (and hides) all system-dependent initialization
/       that must happen before the API may be used.  Ditto for termination
/       that must happen before the application that uses the API can exit.
/
/    How it works:
/       The Unix version saves the current umask on initialization, and
/       sets it to 077 (so that Caucus database files are written so they
/       may only be accessed by the owner).  On termination, it restores
/       the original umask.
/
/    Returns: 
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies: heavy
/
/    Known bugs:      none
/
/    Home:  api/sysainit.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/10/92 20:46 New function. */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "systype.h"

sysainit (phase)
   int    phase;
{


#if UNIX
   static int    umaskval = 99;

   if (phase == 0) {
      umaskval = umask((mode_t) 077);   /* if no mode_t, see types.h. */
   }
   else {
      if (umaskval != 99) umask ((mode_t) umaskval);
   }
#endif

   return (1);
}
