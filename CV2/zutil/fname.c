/*** FNAME.  Program to fix Caucus NAMESxxx files.
/
/    Invoked as:
/       fname  [-c] confid  userids
/
/    Arguments:
/       confid     full path name of Caucus home directory.
/       userids    file containing list of userids, one per line.
/
/    Purpose:
/     1) "FIX"
/       Recreate the Caucus "names" files from the user registration
/       information.
/       FNAME destroys and re-creates the Caucus global name files,
/       MISC/namesNNN.  It uses the information in each user's register
/       (XURG) file to re-create the name files.
/
/     2) "CHECK"
/       If you include the -c option, fname only checks to see if:
/
/       *  Each userid in USERIDS has a XUDD directory
/       *  Each userid in USERIDS has nameparts for each name part in XURG
/          (Note that it does *not* check that each namepart in the NAMES
/           database has a namepart in the XURG.)
/
/       FNAME reports any userids that do not have a XUDD or correct name parts.
/       FNAME does *not* destroy the name files.
/ 
/    How it works:
/     1) "FIX"
/       Fname deletes the MISC/namesNNN files, and then creates an
/       empty set of names files.  It reads the userids one at a time,
/       loads the full name of that user from their XURG file, and
/       calls modnames() to add all of that user's name parts to the
/       names files.
/
/     2) "CHECK"
/       FNAME reads the userids one at a time, verifying the existence of
/       each XURG and correct name parts as it goes.
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
/    Home:  zutil/fname.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/13/90 17:29 New program. */
/*: CR  6/21/90 12:41 Remove unit_wipe() call. */
/*: CR  4/08/91 18:50 Add new unit_lock argument. */
/*: CR  5/02/91 18:37 Remove initsort(). */
/*: CR  7/30/91 18:15 Comment ENTRY, add 3rd arg to loadreg(). */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CR 12/30/91 13:45 Update to CaucusLink norms; new header. */
/*: CR 10/09/92 14:44 Chixify. */
/*: CR  2/02/93 12:44 Use ascquick() of u->name. */
/*: JV  5/19/93 14:44 Add CHECK. */
/*: CP  5/06/92 12:01 Add empty 'item' arg to initnmdir() call. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */
/*: CN  5/16/93 13:27 Add 'cnum' arg to initnmdir(). */
/*: CP  7/14/93 14:32 Integrate 2.5 changes. */
/*: JZ 10/18/93 10:41 Remove dust bunnies, return at end. */
/*: CR 10/24/95 13:46 addu: strip (userid)s, add proper (userid). */
/*: CR 11/12/03 Add entry_test() call to force proper linking. */

#include <stdio.h>
#include "caucus.h"
#include "text.h"
#include "systype.h"

#define USAGE printf ("Usage: fname [-c] confid userid_list\n")

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
   int    i, arg=1, check=0;
   Chix   userid, uidfile;
   char   logdir[256];

   sys_nt();

/* ENTRY ("fname", ""); */
   screen.lines =  0;    used.lines = 0;
   screen.width = 80;    used.width = 0;

   if (argc < 3) {
      USAGE;
      exit (0);
   }

   confid   = chxalloc (L(80), THIN, "fname confid");
   userid   = chxalloc (L(80), THIN, "fname userid");
   uidfile  = chxalloc (L(80), THIN, "fname uidfile");
   confname = chxalloc (L(80), THIN, "fname confname");

   /*** Check for incorrect order of options and args. */
   for (i=2; i< argc; i++)
      if (argv[i][0] == '-') {
         USAGE;
         exit (0);
      }

   if (argv[1][0] == '-') {
      if (argv[1][1] == 'c') check = 1;
      else {
         USAGE;
         exit (0);
      }
      arg=2;
   }

   strcpy (logdir, argv[arg]);
   strcat (logdir, "/SOCKET");
   logger_init (logdir, argv[0], "cau");

   chxcpy (confid, CQ(argv[arg]));  arg++;
   chxcpy (uidfile, CQ(argv[arg]));

   /*** Initialize stuff. */
/* servername[0] = '\0'; */
   systurn (&term_in_caucus, GET_TERM, 0);
   systurn (&term_original,  GET_TERM, 0);

   utilglobals();
   chx_init (ASCII);
   sysexit (-1);
   unitglobals();
   mainglobals();
   sysbrkinit();
   sysglobals();

   thisuser->dict = 0;

   /*** "FIX" */
   if (NOT check) {
      /*** Delete all of the names files. */
      for (i=0;   i<100;   ++i) {
         if (unit_lock (XSNN, WRITE, L(0), L(i), L(0), L(0), nullchix)) {
            unit_kill  (XSNN);
            unit_unlk  (XSNN);
         }
      }
      if (unit_lock (XSND, WRITE, L(0), L(0), L(0), L(0), nullchix)) {
         unit_kill  (XSND);
         unit_unlk  (XSND);
      }
   
      initnmdir (XSND, XSNN, 0, nullchix, L(0));
   
      /*** For each userid in the input file, add that person's name to
      /    the global names files... */
      if (unit_lock (XIRE, READ, L(0), L(0), L(0), L(0), uidfile)) {
         if (unit_view (XIRE)) {
            while (unit_read (XIRE, userid, L(0))) {
               if (sysbrktest())  break;
               chxtrim   (userid);
               jixreduce (userid);
               addu      (userid);
            }
            unit_close (XIRE);
         }
         unit_unlk (XIRE);
      }
   }

   /*** "CHECK" */
   if (check) {
      if (unit_lock (XIRE, READ, L(0), L(0), L(0), L(0), uidfile)) {
         if (unit_view (XIRE)) {
            while (unit_read (XIRE, userid, L(0))) {
               if (sysbrktest())  break;
               chxtrim   (userid);
               jixreduce (userid);
               if (checku(userid) == -1) break;
            }
            unit_close (XIRE);
         }
         unit_unlk (XIRE);
      }
   }

   sysexit (0);
   return  (0);
}

FUNCTION   addu (id)
   Chix    id;
{
   struct namlist_t *add, *del, *nlnode(), *nlnames();
   Userreg           u, make_userreg();
   Chix              word;
   short             c;
   int               success, w, ok;
   int4              pos, paren;
   extern Chix       sitevar[];

   ENTRY ("addu", "");

   printf ("Loading %-16s...", ascquick(id));

   u    = make_userreg();
   word = chxalloc (L(40), THIN, "addu word");
   if (sysudnum (id, -1, 0) == 0  ||  NOT loadreg (id, u, sitevar)) {
      printf ("failed.\n");
      FAIL;
   }

   /*** Find beginning of "(userid)" name part.  If more than one in a
   /    row, find the first. */
   paren = -1;
   for (w=1;   (pos = chxtoken (word, nullchix, w, u->name)) >= 0;   ++w) {
      if (chxvalue (u->name, pos) == '(') {
         if (paren <  0)  paren = pos;
      }
      else {
         if (paren >= 0)  paren = -1;
      }
   }

   /*** Remove any "(userid)" name parts, and add the official one. */
   if (paren >= 0) {
      chxclear  (word);
      chxcatsub (word, u->name, L(0), paren);
      chxcpy    (u->name, word);
   }
   chxformat (u->name, CQ(" (%s)"), L(0), L(0), id, nullchix);
   ok = storereg  (u, 0, sitevar);

   printf ("ok=%2d %-40s", ok, ascquick(u->name));

   /*** Add each word in this user's full name to the global nameparts
   /    directory. */
   del = nlnode  (1);
   add = nlnames (u->name, id);

   c = modnames (XSND, XSNN, nullchix, 0, L(0), add, del, NOPUNCT);
   nlfree (add);
   nlfree (del);
   if (NOT c) {
      printf (" Couldn't add!\n");
      FAIL;
   }
   printf (" OK.\n");
   SUCCEED;

done:
   chxfree (word);
   free_userreg(u);
   RETURN (success);
}



FUNCTION checku (userid)
   Chix userid;
{
   int    nlist_size, userid_match;
   char   userid_str[80];
   Chix   mattag, nlowner;
   struct namlist_t *nl, *regnl, *matches, *nlnames();
   Userreg reg, make_userreg();

   mattag   = chxalloc (L(16), THIN, "fname mattag");
   nlowner  = chxalloc (L(20), THIN, "fname nameparts");


   ascofchx (userid_str, userid, L(0), ALLCHARS);
   printf ("Checking %-16s...", userid_str);

   if (NOT unit_lock (XUDD, READ, L(0), L(0), L(0), L(0), userid)) {
      printf ("Major Database error: can't lock user directory.\n");
      unit_unlk (XUDD);
      return (-1);
   }

   /* Check for user directory ($confid/USERnnn/jvictor). */
   if (NOT unit_check (XUDD)) {
      printf ("No User Directory.\n");
      unit_unlk (XUDD);
      return (0);
   }
   unit_unlk (XUDD);

   /* Check for registration file (XURG) and load it. */
   reg = make_userreg();
   if (NOT loadreg (userid, reg, NULLSITE)) {
      printf ("Couldn't load registration info.\n");
      return (0);
   }

   /* Verify name parts in XURG. */
   regnl = nlnames (reg->name, nlowner);
   nlist_size = nlsize (regnl);
   if (nlist_size == 0) {
      printf ("Registration file has been corrupted.\n");
      nlfree (regnl);
      return (0);
   }

   /* Check consistency of XURG vs. XSNN.  */
   matchnames (XSND, XSNN, 0, userid, regnl, 1, &matches, 1);
   userid_match = 0;
   for (nl = matches->next; nl != null.nl; nl = nl->next) {
      chxtoken (mattag, nullchix, 2, nl->str);
      if (chxeq (mattag, userid)) userid_match = 1;
   }
   if (NOT userid_match)
      printf ("Names in XURG do not match names in XSNN.\n");
   else {
      printf ("%-40s", ascquick(reg->name));
      printf (" OK.\n");
   }

   nlfree (matches);
   nlfree (regnl);
   chxfree (nlowner);
   entry_test ("", "");

   return (0);
}
