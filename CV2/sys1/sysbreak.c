/*** SYSBREAK.  Break (interrupt) handling routines.
/
/  The following routines are used to intercept and announce BREAK, CANCEL,
/  or whatever the vanilla interrupt signal is for a given system.  The
/  visible routines are:
/
/     sysbrkinit()        Initializes interrupt handling; must be executed
/                         once before interrupt testing is to be done.
/
/     sysbrkignore()      Causes "CANCEL" interrupt to be ignored.  Must
/                         call sysbrkinit() again to re-initialize
/                         interrupt handling.
/
/     sysbrktest()        Returns 0 if no interrupt has been encountered,
/                         or a non-zero code if an interrupt has occurred.
/                         The value depends on the type of interrupt.
/
/     sysbrkclear()       Clears the "interrupt has occurred" flag.  Note
/                         that SYSBRKTEST does NOT automatically clear.
/
/     sysbrkset()         Sets the "interrupt has occurred" flag.
/
/ NOTES:
/   (a) currently, if more than MAXQUIT un-processed interrupts are
/       encountered, the interrupt catcher will automatically exit to the
/       operating system with a status of 7.
/
/   (b) sysbrkignore() is provided for systems (such as SunOS) which
/       need to ignore interrupts while doing sensitive operations
/       like creat() or open().
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#define  MAXQUIT  10

/*---------------------------------------------------*/
#if SYSV | HPXA9
#include <signal.h>

static int break_flag = 0;

FUNCTION  sysbrkinit()
{
   int sysbrk2();

   ENTRY ("sysbreak", "");
/* break_flag = 0; */
   signal (SIGINT,  sysbrk2);
   RETURN (1);
}

FUNCTION  sysbrk2()
{
   signal (SIGINT, sysbrk2);
   ++break_flag;
   if (break_flag > MAXQUIT)  sysexit(7);
}

FUNCTION  sysbrkignore()
{
   signal (SIGINT, SIG_IGN);
}

FUNCTION  sysbrkset()
{
   ++break_flag;
}

FUNCTION  sysbrktest()
{
   ENTRY ("sysbrktest", "");
   if (break_flag)  break_flag = 1;
   RETURN (break_flag);
}

FUNCTION  sysbrkclear()
{
   ENTRY ("sysbrkclear", "");
   break_flag = 0;
   RETURN (1);
}
#endif

/*---------------------------------------Berkley Unix 4.2----------------*/
#if BSD21 | FBS22
#include <signal.h>
#include <setjmp.h>

int     sysb_flag = 0;   /* Interrupt flagged? */
int     sysb_read;       /* Interrupt in the middle of a read? */
jmp_buf sysb_env;        /* longjmp environment in case of sysb_read */

FUNCTION  sysbrkinit()
{
   struct sigvec vector;
   int sysbrk2();

   ENTRY ("sysbreak", "");
   vector.sv_handler = sysbrk2;
   vector.sv_mask    = 4;
   vector.sv_onstack = 0;
/* sysb_flag = 0; */
   sysb_read = 0;
   sigvec (SIGINT, &vector, 0);
   RETURN (1);
}

FUNCTION  sysbrk2()
{
   ++sysb_flag;
   if (sysb_flag > MAXQUIT)  sysexit(7);
   if (sysb_read) {
      sysb_read = 0;
      longjmp (sysb_env, 1);
   }
}

FUNCTION  sysbrkignore()
{
   signal (SIGINT, SIG_IGN);
}

FUNCTION  sysbrktest()
{
   ENTRY ("sysbrktest", "");
   if (sysb_flag)  sysb_flag = 1;
   RETURN (sysb_flag);
}

FUNCTION  sysbrkset()
{
   ++sysb_flag;
}

FUNCTION  sysbrkclear()
{
   ENTRY ("sysbrkclear", "");
   sysb_flag = 0;
   RETURN (1);
}
#endif

/*---------------------------------------SunOS---------------------------*/
#if SUN41 | LNX12
#include <signal.h>
#include <setjmp.h>

int     sysb_flag = 0;   /* Interrupt flagged? */
int     sysb_read;       /* Interrupt in the middle of a read? */
jmp_buf sysb_env;        /* longjmp environment in case of sysb_read */

FUNCTION  sysbrkinit()
{
   void   sysbrk2();

   ENTRY ("sysbreak", "");
/* sysb_flag = 0; */
   sysb_read = 0;
   signal (SIGINT, sysbrk2);
   RETURN (1);
}

FUNCTION void sysbrk2()
{
   ++sysb_flag;
#if LNX12
   signal (SIGINT, sysbrk2);
#endif
   if (sysb_flag > MAXQUIT)  sysexit(7);
   if (sysb_read) {
      sysb_read = 0;
      longjmp (sysb_env, 1);
   }
}

FUNCTION  sysbrkignore()
{
   signal (SIGINT, SIG_IGN);
}

FUNCTION  sysbrktest()
{
   ENTRY ("sysbrktest", "");
   if (sysb_flag)  sysb_flag = 1;
   RETURN (sysb_flag);
}

FUNCTION  sysbrkset()
{
   ++sysb_flag;
}

FUNCTION  sysbrkclear()
{
   ENTRY ("sysbrkclear", "");
   sysb_flag = 0;
   RETURN (1);
}
#endif


/*--------------------------------NT---------------------------------*/
#if NUT40 | WNT40
FUNCTION  sysbrkinit() { }


FUNCTION  sysbrkignore() { }

FUNCTION  sysbrkset() { }

FUNCTION  sysbrktest() {
   return (0);
}

FUNCTION  sysbrkclear() { }

#endif
