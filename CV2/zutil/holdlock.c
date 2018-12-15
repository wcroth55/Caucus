/*** HOLDLOCK.  Simple program to test Caucus unitlock functions.
/
/    Invoked as:
/       holdlock  confid
/
/    Arguments:
/       confid     full path name of Caucus home directory.
/
/    Purpose:
/       HOLDLOCK is used to test the basic Caucus locking function,
/       presumably during a port to a new system.
/
/    How it works:
/       HOLDLOCK locks XSCD, the caucus-wide conference names
/       directory.  It keeps it locked until the user presses RETURN.
/
/       To use: one person (process) should run HOLDLOCK, and wait.
/       A second person (process) should run HOLDLOCK... which, if
/       locking works OK, should not be granted the lock until the
/       first person presses RETURN (ending HOLDLOCK).
/
/    Exit status: 0 on success, 1 on failure.
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  zutil/holdlock.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/20/95 16:27 New program. */
/*: CR 11/12/03 Add entry_test() call to force proper linking. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "text.h"

extern struct screen_template  used, screen;
extern struct termstate_t      term_original, term_in_caucus;
extern Chix                    confid;
extern int                     debug;

FUNCTION  main (argc, argv)
   int    argc;
   char  *argv[];
{
   char   str[200], logdir[256];
   Chix   userid;

   screen.lines = 23;    used.lines = 0;
   screen.width = 80;    used.width = 0;

   /*** Check arguments and initialize CONFID. */
   if (argc < 2) {
      printf ("Usage: holdlock  confid  [-d]\n");
      exit   (1);
   }
   if (argc == 3)  debug = -1;

   strcpy (logdir, argv[1]);
   strcat (logdir, "/SOCKET");
   logger_init (logdir, argv[0], "cau");

   userid = chxalloc (L(80), THIN, "holdlock userid");
   confid = chxalloc (L(80), THIN, "holdlock confid");
   chxofascii (confid, argv[1]);

   systurn (&term_original,  GET_TERM, 0);
   systurn (&term_in_caucus, GET_TERM, 0);
   utilglobals();
   optchk  ();

   chx_init (ASCII);

   /*** Miscellaneous initialization. */
   sysexit (-1);
   unitglobals();
   mainglobals();
   sysbrkinit();
   sysglobals();
   sysuserid (userid);
   sysdirchk (userid);

   printf ("\n");
   printf ("Requesting Read Lock...\n\n");
   if (NOT unit_lock (XSCD, READ, L(0), L(0), L(0), L(0), nullchix)) {
      printf  ("Unit_lock (XCSD) failed.\n");
      sysexit (0);
   }

   printf ("Read Lock granted.  Press RETURN to release.\n");
   gets   (str);
   printf ("\n");
   unit_unlk (XSCD);

   printf ("Press RETURN to request Write Lock: ");
   fflush (stdout);
   gets   (str);
   printf ("\n");

   if (NOT unit_lock (XSCD, WRITE, L(0), L(0), L(0), L(0), nullchix)) {
      printf  ("Unit_lock (XCSD) failed.\n");
      sysexit (0);
   }

   printf ("Write Lock granted.  Press RETURN to release: ");
   fflush (stdout);
   gets   (str);
   unit_unlk (XSCD);
   printf ("\n");

   sysexit   (0);
   entry_test ("", "");
}
