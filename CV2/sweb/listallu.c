
/*** LISTALLU.   Produce list of all Caucus users.
/
/    Invoked as:
/       listallu   confid
/
/    Arguments:
/       confid     full path name of Caucus home directory.
/
/    Purpose:
/       Listallu is a Caucus maintenance support program.  It 
/       produces a list of *all* Caucus users in a particular
/       format that is in turn used by other utilities or
/       CML scripts.
/
/    How it works:
/       Writes a line for each user, with (epoch) time 'last on',
/       userid, and full name.
/
/       Also writes a line marked as epoch time 0, with the
/       epoch time when the file was written, and the approximate
/       time to compute the whole file.
/
/    Exit status:
/
/    Known bugs:      none
/
/    Home:  sweb/listallu.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/17/98 15:41 New program. */
/*: CR 11/12/03 Add entry_test() call to force proper linking. */

#include <stdio.h>
#include <time.h>
#include "chixuse.h"
#include "api.h"
#include "common.h"
#include "caucus.h"
#include "unitwipe.h"

extern struct screen_template  used, screen;

main (argc, argv)
   int   argc;
   char *argv[];
{
   Initapi  init;
   Person   person;
   int      start, usernnn, ok, error;
   int4     laston, epoch_of();
   int4     t0, t1;
   int      year, month, day, hour, minute, sec;
   char     fullname[200], userdir[256], uid[80], logdir[256];

   if (argc < 2) {
      fprintf (stderr, "Usage: listallu confid\n");
      exit    (1);
   }

   strcpy (logdir, argv[1]);
   strcat (logdir, "/SOCKET");
   logger_init (logdir, argv[0], "cau");

   /*** Initialize the API. */
   t0 = time(NULL);
   init = a_mak_init ("listallu init");
   chxofascii (init->confid, argv[1]);
   a_init (init);

   person = a_mak_person ("");

   screen.lines =  0;    used.lines = 0;
   screen.width = 80;    used.width = 0;

   /*** Scan all the people... */
   sysbrkinit();
   for (usernnn=1;   (1);   ++usernnn) {
      if (sysbrktest())   break;

      sprintf (userdir, "%s/USER%03d", argv[1], usernnn);

      for (start=1;   sysgfdir(userdir, uid, start, &error) == 1;   start=0) {

         /*** Get the information about USER. */
         chxcpy (person->owner, CQ(uid));
         ok = a_get_person (person, P_EVERY, A_WAIT);
         if (ok != A_OK)  continue;
   
         ascofchx (fullname, person->name, L(0), L(200));
         laston = epoch_of (person->laston);
         printf ("%11d %22s %s\n", laston, uid, fullname);
      }
      if (start == 1)  break;
   }

   t1 = time(NULL);
   printf ("%11d %22d %d\n", 0, t1, t1-t0);

   entry_test ("", "");
   exit   (0);
}
