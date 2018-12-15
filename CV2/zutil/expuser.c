/*** EXPUSER:  Delete users who haven't used Caucus since a particular date.
/
/    Invoked as:
/       expuser_x  confid  opt  date
/
/    Arguments:
/       confid     full path name of Caucus home directory.
/       opt        -p (print list of users), or -r (delete users)...
/       date       ...who haven't used Caucus since this date.
/
/    Purpose:
/       Expuser_x is a Caucus maintenance support program.  It lists
/       or deletes all users who have not accessed Caucus since a
/       particular date.
/
/    How it works:
/       DATE may be of the form "mm/dd/yy" or "dd-MMM-yy".
/
/       If no date is given for "-p", all users are displayed.
/
/    Exit status:
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  zutil/expuser.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  8/20/93 12:00 Create this. */
/*: CK  9/28/93  9:43 Add header comments, api initialization. */
/*: JZ 10/18/93 11:15 Fix ascofchx args. */
/*: CP  1/20/94 18:01 Make screensize 0, but test for interrupts. */
/*: CR 11/12/03 Add entry_test() call to force proper linking. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "common.h"
#include "caucus.h"
#include "unitwipe.h"

extern struct screen_template  used, screen;

#define USAGE  USAGE1;  USAGE2
#define USAGE1 printf ("Usage: expuser confid -r date\n")
#define USAGE2 printf ("       expuser confid -p date\n")

main (argc, argv)
   int   argc;
   char *argv[];
{
   Initapi  init;
   char     fullname[200], logdir[256];
   int      ok, print=0, kill=0, success;
   int4     time=0, laston, time_of_date();
   Chix     user, ctime, output;
   Person   person;

   if (argc < 2) {  USAGE;  exit(1);  }

   strcpy (logdir, argv[1]);
   strcat (logdir, "/SOCKET");
   logger_init (logdir, argv[0], "cau");

   /*** Initialize the API. */
   init = a_mak_init ("expuser init");
   chxofascii (init->confid, argv[1]);
   a_init (init);

   ctime  = chxalloc (L(16), THIN, "expuser ctime");
   output = chxalloc (L(80), THIN, "expuser output");
   user   = chxalloc (L(16), THIN, "expuser user");

   person = a_mak_person ("");

   screen.lines = 0;    used.lines = 0;
   screen.width = 80;    used.width = 0;

   if (argc >= 3) {
      switch (argv[2][1]) {
      case 'p':
         print=1;
         if (argc > 3) {
            chxofascii (ctime, argv[3]);
            time = time_of_date (ctime, nullchix);
         }
         break;

      case 'r':
         kill=1;
         if (argc < 4) { USAGE;   FAIL; }
         chxofascii (ctime, argv[3]);
         time = time_of_date (ctime, nullchix);
         break;

      default:
         USAGE;   FAIL;
      }
   }
/* utilglobals(); */

   
   /*** Scan all the people... */
   sysbrkinit();
   chxclear (user);
   while (nextuser (user) > 0) {
      if (sysbrktest())   break;

      /*** Get the information about USER. */
      chxcpy (person->owner, user);
      ok = a_get_person (person, P_EVERY, A_WAIT);
      if (ok != A_OK) {
         chxclear  (output);
         chxformat (output, CQ("Couldn't get user: %s.\n"), L(0), L(0),
                    person->owner, nullchix);
         unit_write (XITT, output);
         continue;
      }
      ascofchx (fullname, person->name, L(0), L(200));
      laston = time_of_date (person->laston, nullchix);

      /*** Display (-p) info about them. */
      if (print  && (time==0 || time > laston) ) {
         chxclear  (output);
         chxformat (output, CQ("%-50s %s\n"), L(0), L(0),
                    chxquick (fullname, 1), person->laston);
         unit_write (XITT, output);
      }

      /*** Delete (-r) them. */
      if (kill  && (time > laston)) {
         chxclear  (output);
         chxformat (output, CQ("Removing user: %s...\n"), L(0), L(0),
                    chxquick (fullname, 1), nullchix);
         unit_write (XITT, output);
         a_del_person (person, L(0), A_WAIT);
      }
   }

done:
   chxfree (ctime);
   chxfree (output);
   chxfree (user);
   a_fre_person (person);
   a_fre_init   (init);
   entry_test ("", "");
   exit (success ? 0 : 1);
}
