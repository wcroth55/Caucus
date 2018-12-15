/*** ADDLEN.  Program to add text length information to each response.
/
/    Invoked as:
/       addlen confid cname
/
/    Arguments:
/       confid     full path name of Caucus home directory.
/       cname      name of conf which should be changed
/
/    Purpose:
/     Caucus V2.5 added support for text length information.
/     However, it only computes and writes out length info for new resp's
/     and resp's which are being changed.
/
/     This program adds the text length info to all responses in a conf.
/   
/    How it works:
/      Get each response that exists with a_get_resp(), put it back with
/      a_chg_resp().
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
/    Home:  zutil/addlen.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  6/06/93 12:10 Create this. */
/*: CP  7/31/93 13:29 a_fre_resp() before finishing. */
/*: CP  8/01/93 10:50 Add a_mak...() name arg. */
/*: CR 11/12/03 Add entry_test() call to force proper linking. */

#include <stdio.h>
#include "caucus.h"
#include "text.h"
#include "systype.h"
#include "api.h"

#define USAGE printf ("Usage: addlen conf cname\n")

extern struct screen_template  used, screen;
extern struct flag_template    flag;
extern struct namlist_t       *sysmgrprint;
extern struct termstate_t      term_in_caucus, term_original;
extern Chix                    confid, confname;
extern Chix                    servername;
extern int                     debug;
extern Userreg                 thisuser;
extern union  null_t           null;

FUNCTION  main (argc, argv)
   int    argc;
   char  *argv[];
{
   int    i, r, cnum, ok, openval;
   Chix   cname;
   struct master_template master;
   Resp    resp;
   Initapi init, a_mak_init();
   char    logdir[256];

   sys_nt();

   screen.lines =  0;    used.lines = 0;
   screen.width = 80;    used.width = 0;

   if (argc < 3) {
      USAGE;
      exit (0);
   }

   strcpy (logdir, argv[1]);
   strcat (logdir, "/SOCKET");
   logger_init (logdir, argv[0], "cau");

   confid   = chxalloc (L(80), THIN, "fname confid");
   cname    = chxalloc (L(80), THIN, "fname cname");
   confname = chxalloc (L(80), THIN, "fname confname");

   /*** Check for incorrect order of options and args. */
   chxcpy (confid, CQ(argv[1]));
   chxcpy (cname,  CQ(argv[2]));

   /*** Initialize stuff. */
   systurn (&term_in_caucus, GET_TERM, 0);
   systurn (&term_original,  GET_TERM, 0);

   /*** Initialize the API. */
   init = a_mak_init("addlen init");
   chxcpy (init->confid, confid);
   a_init (init);

   utilglobals();
   chx_init (ASCII);
   sysexit (-1);
   unitglobals();
   mainglobals();
   sysbrkinit();
   sysglobals();

   thisuser->dict = 0;

   cnum = cnumber (cname, LOCALCONF, &openval);
   if (cnum == -1) {
      printf ("There is no conference by that name.\n");
      sysexit (0);
      return (0);
   }
   if (cnum == 0) {
      printf ("That conference name is ambiguous.\n");
      printf ("Please use more letters.\n");
      sysexit (0);
      return (0);
   }

   resp = a_mak_resp("addlen resp");
   resp->cnum = cnum;

   /*** Load the masteres file. */
   make_master (&master, MAXITEMS);
   srvldmas (1, &master, cnum);

   /*** Go through each item inf the conference. */
   for (i=1; i<= master.items; i++) {
      if (master.responses[i] == -1) continue;
      resp->inum = i;

      /*** Go through each response in the item. */
      for (r=0; r<= master.responses[i]; r++) {
         resp->rnum = r;
         ok = a_get_resp (resp, P_RESP, A_WAIT);
         if (ok == A_NORESP) continue;
         if (ok != A_OK) {
            printf ("There is a problem with the database for this");
            printf ("conference in item %d, response %d\n", i, r);
      printf ("Addlen is exiting at that point to minimize further damage.\n");
            return (0);
         }
         a_chg_resp (resp, P_RESP, A_WAIT);
      }
   }

   a_fre_resp (resp);
   a_exit ();
   a_fre_init (init);
   sysexit (0);
   entry_test ("", "");
   return  (0);
}



