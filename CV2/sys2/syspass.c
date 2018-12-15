
/*** SYSPASS.   Change the user's password.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include <sys/types.h>
#include "caucus.h"
#include "sys.h"
#include "systype.h"

#if UNIX
#include <unistd.h>
#endif

extern struct flag_template flag;
extern struct termstate_t   term_in_caucus, term_original;

FUNCTION  syspass()
{

/*-------------------------------NT-----------------------------*/
#if NUT40 | WNT40
   return (0);
#endif


/*-------------------------------SYSV-ish Unix------------------*/
#if SYSV | HPXA9 | BSD21 | LNX12 | FBS22
   int   status, child;

   ENTRY ("syspass", "");

   /*** Spawn off a task which sets the effective id to the real id,
   /    exec /etc/passwd. */
   systurn (&term_in_caucus, GET_TERM, 0);
   systurn (&term_original,  SET_TERM, 0);
   child  = fork();
   if      (child == -1)  {
      systurn (&term_in_caucus, SET_TERM, 0);
      RETURN  (0);
   }

   /*** The parent task waits for the child to terminate. */
   else if (child >   0)
      while (wait (&status) != child)  { if (syshngtest()) break; }

   else {
      setuid ((uid_t) getuid());
      execl  ("/bin/passwd", "passwd", NULL);
      exit  (0);
   }

   systurn (&term_in_caucus, SET_TERM, 0);

   RETURN ( (status & 0xFF00) ? 0 : 1);
#endif

/*-------------------------Berkeley Unix-------------------------*/
#if SUN41
   int   status, child;

   ENTRY ("syspass", "");

   /*** Spawn off a task which sets the effective id to the real id,
   /    exec /etc/passwd. */
   systurn (&term_in_caucus, GET_TERM, 0);
   systurn (&term_original,  SET_TERM, 0);
   child  = vfork();
   if      (child == -1)  {
      systurn (&term_in_caucus, SET_TERM, 0);
      RETURN  (0);
   }

   /*** The parent task waits for the child to terminate. */
   else if (child >   0)
      while (wait (&status) != child)  { if (syshngtest()) break; }

   else {
      setreuid ((uid_t) getuid(), (uid_t) getuid());
      execl  ("/bin/passwd", "passwd", NULL);
      exit  (0);
   }

   systurn (&term_in_caucus, SET_TERM, 0);
   RETURN ( (status & 0xFF00) ? 0 : 1);
#endif

}
