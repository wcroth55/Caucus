
/*** SYSPRV.  Turn access to Caucus data files on (1) or off (0).
/
/    ok = sysprv (choice);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    choice;      (1=on, 0=off)
/
/    Purpose:
/       Turns privileged access to Caucus data files on (1) or off (0).
/
/    How it works:
/       This means different things for different operating systems.
/   
/       For UNIX, access to the Caucus data files is done with the set
/       uid bit.  Normally Caucus runs as the "caucus" user.  For some
/       operations (again, i/o redirection, EDIT at AND NOW) Caucus
/       temporarily reverts to the real userid, and afterwards switches
/       back to the effective userid.
/   
/       For all other systems, SYSPRV does nothing and always returns 1. 
/
/
/    Returns: 1 on success
/             0 on error
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
/    Notes:
/       MUST NOT USE CHIX!  Some chix debugging functions call this,
/       which could set up a infinite stack-devouring loop.
/
/    Home:  sys2/sysprv.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:59 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include <sys/types.h>
#include "caucus.h"
#include "systype.h"

extern int    debug;

FUNCTION sysprv (choice)
   int   choice;
{

/*---------------------------UNIX--------------------------*/
#if UNIX
   uid_t getuid(), geteuid();
   static int rid = -1, eid = -1;
   int        success;
  
   if (rid < 0) {
      rid = getuid ();
      eid = geteuid();
   }

#if SUN41 | BSD21 | FBS22
   success = (choice==0 ? seteuid ((uid_t) rid) : seteuid ((uid_t) eid) ) >= 0;
#else
   success = (choice==0 ? setuid  ((uid_t) rid) : setuid  ((uid_t) eid) ) >= 0;
#endif
  
   if (debug & DEB_PRV) printf ("sysprv(%d), success=%d\n", choice, success);

   return (success);
#endif


/*---------------------------NT----------------------------*/
#if NUT40 | WNT40
   return (1);
#endif

}
