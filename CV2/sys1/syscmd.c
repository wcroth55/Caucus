/*** SYSCMD.  Execute system COMMAND as though it had been typed by user.
/
/   ok = syscmd (command, should_wait);
/
/   Parameters:
/     int   ok;            (Return value, see below)
/     char *command;       (Operating system command)
/     int   should_wait;   (if SYS_WAIT, wait for command to finish)
/
/   Purpose:
/      SYSCMD allows us to invoke operating system commands from within
/      Caucus or one of the CaucusLink programs.
/
/   How it works:
/      Lines beginning with "#" are treated as comments and ignored.
/
/      If WAIT is SYS_WAIT, syscmd() returns after the COMMAND is
/      complete.  Otherwise it returns immediately.  (Note: not all
/      systems support the concept of not waiting.)
/
/   Returns:
/      There are five categories of return values for syscmd():
/         A) Should_wait is not SYS_WAIT.  Returns 1 immediately.
/
/      All other cases assume should_wait == SYS_WAIT.
/
/         A) Command (appears to have) executed successfully (this includes
/            comments).
/            Returns: the exit value PLUS ONE.
/
/         B) Command failed: unknown error
/            Returns: 0
/
/         C) Command failed or couldn't be started
/            Returns negative even number:
/            -2: COMMAND specifies an incorrect command or file name
/            -4: User is not allowed to execute the command or file
/            -6: Argument list too big
/
/         D) Command failed or couldn't be started due to a temporary
/            condition (e.g. temporary resource limitation)
/            Returns negative odd number:
/            -1  O.S. error: Too many processes
/            -3: Not enough memory
/            -5: Child terminated by another process
/
/      These groupings are provided in this way so that the calling
/      program can evaluate reality quickly and easily:
/
/      1) If the return value is greater than zero, the command executed.
/         Otherwise, it didn't even run.
/         The caller can also check the return value from the command
/         by subtracting one from syscmd()'s return value.
/      2) If it failed to run, check the low-order bit:
/         1) If it's 1, it may be useful to try again in a few seconds.
/         2) If it's 0, trying again won't help.
/
/ **Errors:
/ **   WARNING: So far this more detailed version has only been implemented
/ **   for A3, IM, SU, SX, and VV!
/
/   Related functions:  sysscrip.c
/
/   Called by: gen/printit.c  main/interpre.c sys/sysclrsc.c
/              sys/sysedito.c sys/sysmstra.c  sys/syspass.c  sys/sysscrip.c
/
/   Operating System Dependencies:
/      The amount of information returned about the child is highly
/      system-dependent.  Unix provides a few different failure conditions,
/      as does DOS, but we don't know what VM returns!
/
/      See the bottom of this file for a more detailed explanation of what
/      is available per O.S.
/
/      Some systems support the WAIT argument.  These include:
/         UNIX
/      These do not support the WAIT argument:
/         DOS, VMS (VMS can, but the code hasn't been modified yet)
/
/   Home: sys1/syscmd.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include <sys/types.h>
#include "caucus.h"
#include "sys.h"
#include "systype.h"

#if UNIX
#include <unistd.h>
#endif

#if SYSV | HPXA9 | BSD21 | LNX12 | FBS22 | SUN41
#include <errno.h>
#endif

extern struct flag_template flag;
extern struct termstate_t   term_original, term_in_caucus;
extern int                  debug;

FUNCTION  syscmd (command, should_wait)
   char  command[];
   int   should_wait;
{

/*-----------------------------NT----------------------------------------*/
#if NUT40
   return (1);
#endif

#if WNT40
   system (command);
   return (1);
#endif


/*-----------------------------SYSV Unix---------------------------------*/
#if SYSV | HPXA9 | BSD21 | LNX12 | FBS22

   int   status, child;

   ENTRY ("syscmd", "");
   if (command[0] == '#')  RETURN (1);

   /*** The Unix version spawns a task that reverts to the Real userid,
   /    and then tries to execute COMMAND via the Bourne shell. */
   systurn (&term_in_caucus, GET_TERM, 0);
   systurn (&term_original,  SET_TERM, 0);
   child  = fork();
   if (child == -1) {
      systurn (&term_in_caucus, SET_TERM, 0);
      if (errno == EAGAIN) RETURN (-1);
      if (errno == ENOMEM) RETURN (-3);
      RETURN (0);
   }

   /*** The parent task waits for the child to terminate. */
   else if (child >   0) {
      if (should_wait == SYS_WAIT)
         while (wait (&status) != child) { if (syshngtest())  break; }
   }
 
   /*** The child task reverts to the Real uid, and executes the
   /    command via the Bourne shell. */
   else {
      setuid ((uid_t) getuid());
      execl  ("/bin/sh", "sh", "-c", command, NULL);
      exit   (1);
   }

   systurn (&term_in_caucus, SET_TERM, 0);

   if (should_wait == SYS_WAIT) {
      RETURN  ( (status & 0xFF00) ? 0 : 1);
   }
   RETURN (1);
#endif


/*-----------------------------Berkeley Unix---------------------*/
#if SUN41
   int   status, child;

   ENTRY ("syscmd", "");
   if (command[0] == '#')  RETURN (1);

   /*** The Unix version spawns a task that reverts to the Real userid,
   /    and then tried to execute COMMAND via the Bourne shell. */
   systurn (&term_in_caucus, GET_TERM, 0);
   systurn (&term_original,  SET_TERM, 0);
   child  = vfork();
   if (child == -1) {
      systurn (&term_in_caucus, SET_TERM, 0);
      if (errno == EAGAIN) RETURN (-1);
      if (errno == ENOMEM) RETURN (-3);
      RETURN  (0);
   }

   /*** The parent task waits for the child to terminate. */
   else if (child >   0) {
      if (should_wait == SYS_WAIT) 
         while (wait (&status) != child) { if (syshngtest())  break; }
   }
 
   /*** The child task reverts to the Real uid, and executes the
   /    command via the Bourne shell. */
   else {
      setreuid ((uid_t) getuid(), (uid_t) getuid());
      execl  ("/bin/sh", "sh", "-c", command, NULL);
      _exit  (1);
   }

   /*** If the child process exited with a status other than 0,
   /    the command failed. */
   systurn (&term_in_caucus, SET_TERM, 0);

   if (should_wait == SYS_WAIT) {
#if SUN41
      /*** Child process was stopped. */
      if ((status & ~0xFF00) == 0177)
         RETURN (0);

      /*** Child process terminated by signal. */
      /*   Note: the man pages for wait() are wrong.  Try it yourself! */
      if ((status & ~0x7FFF) == 1)
         RETURN (-5);

      /*** Probably finished successfully. */
      RETURN ((status & ~0x00FF) + 1);
#endif
   }
   RETURN (1);
#endif

}
