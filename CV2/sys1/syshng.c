
/*** SYSHNG.  Hangup detection functions:
/
/    syshnginit (detect);
/    h = syshngtest ();
/
/    Parameters:
/       int    detect;   Detect hangups  (1), ignore hangups (0).
/       int    h;        Hangup detected (1), else 0.
/
/    Purpose:
/       Syshnginit() controls hangup detection and handling.  If DETECT
/       is 1, hangup signals are trapped and detected.  If DETECT is 0,
/       hangup signals are ignored.  Syshnginit() may be called multiple
/       times to change how hangups are treated.
/
/       Syshngtest() tells the caller if a hangup has occurred and been
/       detected.
/
/    How it works:  
/
/    Returns:  Syshnginit() always returns 1.
/       Syshngtest() returns 1 if hangups are detected, AND a hangup
/       has occurred.  It returns 0 if hangups are ignored, or if no
/       hangup has occurred.
/
/    Error conditions:
/
/    Side effects:  Some versions are coupled via setjmp() and longjmp()
/       with sysread(), as a hangup may not cause read() to return 
/       automatically.
/
/    Related functions: sysread().
/
/    Called by:
/
/    Operating system dependencies:
/       Sys V unix: use signal().
/       PRIME:      see SYSINIT in sysinit.h.
/       Others:     not implemented.  Syshngtest() always returns 0.
/
/    Known bugs:      none
/
/    Home:  sys/syshng.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/01/91 14:26 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#define  TYPE  int

#if SUN41
#include <setjmp.h>
extern int     sysb_flag;   /* Interrupt flagged? */
extern int     sysb_read;   /* Interrupt in the middle of a read? */
extern jmp_buf sysb_env;    /* longjmp environment in case of sysb_read */
#undef   TYPE
#define  TYPE  void
#endif

static int hangup = 0;
static int detect = 1;

/*-------------------------------- Unixes--------------------------*/
#if UNIX

#include <signal.h>

FUNCTION  syshnginit (on)
   int    on;
{
   TYPE  sysbrk2(), syshang();

   ENTRY ("syshnginit", "");
   hangup = 0;
   detect = on;
   if (detect) { signal (SIGHUP, syshang);    signal (SIGQUIT, syshang); }
   else        { signal (SIGHUP, SIG_IGN);    signal (SIGQUIT, SIG_IGN); }
   RETURN (1);
}

FUNCTION  syshngtest ()
{
   ENTRY  ("syshngtest", "");
   RETURN (detect ? hangup : 0);
}

FUNCTION  TYPE syshang()
{
   TYPE sysalarm();
   hangup = 1;
   signal (SIGALRM, sysalarm);
   alarm  (3);

#if SUN41
   if (sysb_read) {
      sysb_read = 0;
      longjmp (sysb_env, 1);
   }
#endif
}

FUNCTION TYPE sysalarm()
{
}
#endif


/*----------------------------------------------------------*/
#if NUT40 | WNT40

FUNCTION  syshnginit (on)
   int    on;
{
   ENTRY  ("syshnginit", "");
   RETURN (1);
}

FUNCTION  syshngtest ()
{
   ENTRY  ("syshngtest", "");
   RETURN (hangup);
}
#endif
