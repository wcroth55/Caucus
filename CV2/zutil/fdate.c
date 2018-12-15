
/*** FDATE.   Fix (create) response & item date-entry-files for a conference.
/
/    Invoked as:
/       fdate   caucus_id  cnumber   [-d]
/
/    Arguments:
/       caucus_id    full pathname of directory containing Caucus database.
/       cnumber      conference number.
/       -d           optional debugging flag
/
/    Purpose:
/       Delete any existing date-entry files for a conference.  Then (re)create
/       those files from scratch by reading the dates from the actual
/       items and responses.
/
/    How it works:
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
/    Known bugs:
/
/    Home:  zfdate/fdate.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CN  5/11/93 23:49 New program. */
/*: CP  7/14/93 17:16 Removed temp, answer. */
/*: CP  8/01/93 10:50 Add a_mak...() name arg. */
/*: CR 11/12/03 Add entry_test() call to force proper linking. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "caucus.h"

main (argc, argv)
   int    argc;
   char  *argv[];
{
   int      cnum, ok, debug;
   Initapi  init, a_mak_init();
   Person   person;
   Cstat    cstat;
   Resp     resp;
   int4     pos;
   Chix     day, time;
   int      success, i, rnum;
   char     logdir[256];

   if (argc < 3) {
      printf ("usage:  fdate  caucus_dir  cnumber\n");
      exit   (1);
   }
   debug = (argc == 4);

   strcpy (logdir, argv[1]);
   strcat (logdir, "/SOCKET");
   logger_init (logdir, argv[0], "cau");

   day  = chxalloc (L(40), THIN, "fdate day");
   time = chxalloc (L(40), THIN, "fdate time");

   /*** Initialize the API. */
   init = a_mak_init("fdate init");
   chxcpy (init->confid, chxquick(argv[1], 0));
   a_init (init);

   /*** Parse the conference number. */
   pos  = 0;
   if (chxvalue (CQ(argv[2]), L(0)) == 'C')  pos = 1;
   cnum = chxint4 (CQ(argv[2]), &pos);

   /*** Create the objects we'll need. */
   person = a_mak_person("fdate person");
   cstat  = a_mak_cstat ("fdate cstat");
   resp   = a_mak_resp  ("fdate resp");

   /*** The 'caucus' user must be registered in order to use FDATE. */
   sysuserid (person->owner);
   ok = a_get_person (person, P_EVERY, A_WAIT);
   if (ok != A_OK) {
      printf ("Userid '%s' is not registered with Caucus.\n", 
                 ascquick(person->owner));
      FAIL;
   }


   /*** Get conference status. */
   cstat->cnum = resp->cnum = cnum;
   ok = a_get_cstat (cstat, P_EVERY, A_WAIT);
   if (ok != A_OK) {
      printf ("Conference %d: can't load status.\n", cnum);
      FAIL;
   }

   /*** Delete/recreate XCED/N item-entry namelist file. */
   delnames  (XCED, XCEN, cnum, L(0));
   initnmdir (XCED, XCEN, cnum, nullchix, L(0));

   /*** For each item... */
   for (i=1;   i<=cstat->items;   ++i) {
      if (cstat->resps[i] < 0)  continue;

      /*** Get response 0. */
      resp->inum = i;
      resp->rnum = 0;
      ok = a_get_resp (resp, P_TITLE, A_WAIT);
      if (ok != A_OK)            continue;

      /*** Add time-entered entry for this item to XCED/N. */
      chxtoken (day,  nullchix, 1, resp->date);
      chxtoken (time, nullchix, 2, resp->date);
      moddate  (1, XCED, XCEN, cnum, L(0), day, time, L(i));
      if (debug) printf ("%3d:     '%s'\n", i, ascquick(day));

      /*** Delete/recreate the resp-time-entry file. */
      delnames  (XCFD, XCFN, cnum, L(i));
      initnmdir (XCFD, XCFN, cnum, nullchix, L(i));

      /*** For each response... */
      for (rnum=1;   rnum<=cstat->resps[i];   ++rnum) {
         resp->rnum = rnum;
         ok = a_get_resp (resp, L(0), A_WAIT);
         if (ok != A_OK) continue;
   
         /*** Add entry for this response to XCFD/N. */
         chxtoken (day,  nullchix, 1, resp->date);
         if (EMPTYCHX(day))  continue;
         chxtoken (time, nullchix, 2, resp->date);
         moddate  (1, XCFD, XCFN, cnum, L(i), day, time, L(rnum));
         if (debug) printf ("%3d.%03d: '%s'\n", i, rnum, ascquick(day));
      }
   }

done:
   chxfree (day);
   chxfree (time);
   a_fre_person (person);
   a_fre_cstat  (cstat);
   a_fre_resp   (resp);
   a_exit();
   a_fre_init   (init);
   entry_test ("", "");
}
