
/*** SWEBA.   Caucus 5.x stand-alone CML interpreter.
/
/    Invoked as:
/       sweba   conf  cmlfile   [arg1 arg2 ... ] [ -l userid pw ]
/
/    Arguments:
/       conf     sweba configuration file
/       cmlfile  CML file to interpret (relative to CML_Path in 'conf')
/       arg1     first argument (as in $arg(1)) to 'cmlfile'.
/       arg2     etc.
/       userid   inside-caucus userid "to be"
/       pw       password
/
/    Purpose:
/       Interpret and execute a CML script in "stand-alone" mode,
/       i.e. as a regular program run from the shell (as opposed
/       to being driven from a web browser).
/
/    How it works:
/       If "-l" option specified, tries to verify userid & pw.  If succeeds,
/       sets userid as specified.  If fails, exits immediately.
/       If no "-l", caucus userid is same as effective unix userid.
/
/    Exit status:
/       0 on success
/       1 on failure (bad arguments)
/       2 on failure (can't read swebd.conf file)
/       3 on failure (a_init() call fails)
/       5 on failure (-l userid/pw does not authenticate)
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  sweba.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/20/96 13:49 New program. */
/*: CR  3/01/02 14:00 Add 'found' argument loadcml(). */
/*: CR  3/21/03 Add is_sweba() function. */
/*: CR  5/25/03 Add sql_init() call. */
/*: CR 10/31/03 Handle no SQL connectstr w/o exiting! */
/*: CR 11/12/03 Add entry_test() call to force proper linking. */
/*: CR 03/06/04 Add "-l userid pw" option to set inside-caucus userid. */
/*: CR 05/16/04 Add version # display to usage message. */
/*: CR 02/15/05 Handle swebd.conf stderr parameter. */
/*: CR 04/06/05 stderrTo "off" must go to /dev/null */
/*: CR 12/03/05 Complain loudly if cannot access SQL tables. */

#include <stdio.h>
#include "chixuse.h"
#include "caucus.h"
#include "sweb.h"
#include "api.h"
#include "systype.h"
#include "sqlcaucus.h"

#if WNT40
#include <windows.h>
#include <iostream.h>
#endif

/*** Define globals needed by other functions (even if not used). */
FILE              *sublog     = NULL;
int                cml_debug  = 0;
int                cml_errors = 1;
struct http_t      http;

#define  BUFMAX 30000
AsciiBuf sqlbuf;

char copyright[] =
"Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved. "
"This program is published under the terms of the GPL; see "
"the file license.txt in the same directory.";

int    output_default = 1;

extern struct flag_template    flag;

FUNCTION  main (int argc, char *argv[])
{
   static Vartab              vars, form, macs;
   static struct  sweb_conf_t sweb_conf;
   static CML_File            cfile;
   CML_File                   make_cml();
   static Obtab               otab;
   Obtab                      make_obtab();
   static Chix                errtext;
   Initapi                    api;
   int                        lice_code, num, size, broke, contd, found;
   int                        sqlrc;
/* int                        cd = 1; */
   Hose                       cd;
   Chix                       empty, id_pw, verified;
   char                       cmlname[200], query[200], temp[400];
   Target                     tg, target();
   char                      *caucus_version();
   AsciiBuf                   ascii_buf();
   ENTRY ("sweba main", "");

#if UNIX
   cd = 1;
#endif
#if WNT40
   cd = GetStdHandle (STD_OUTPUT_HANDLE);
#endif

   if (argc < 3) {
      fprintf (stderr, "Version: %s\n"
               "Usage: sweba conf cmlfile [-l userid pw]\n", caucus_version());
      exit    (1);
   }

   sqlbuf = ascii_buf (NULL, BUFMAX);

   /*** Load the SWEBD configuration file. */
   strcpy (sweb_conf.filename, argv[1]);
   if (NOT load_conf (&sweb_conf)) {
      fprintf (stderr, "Cannot read configuration file %s.\n", argv[1]);
      exit    (2);
   }

   /*** Handle stderr parameter */
   if      (streq  (sweb_conf.stderrTo, "on")) ;
   else if (streq  (sweb_conf.stderrTo, "off")  ||
            NULLSTR(sweb_conf.stderrTo))  freopen ("/dev/null",        "w", stderr);
   else                                   freopen (sweb_conf.stderrTo, "a", stderr);

   empty    = chxalloc (  L(4), THIN, "process empty");
   errtext  = chxalloc (L(256), THIN, "process errtext");
   id_pw    = chxalloc (L( 20), THIN, "process id_pw");
   verified = chxalloc (L( 20), THIN, "process verified");

   /*** Initialize SQL connection to the database. */
   if ( sweb_conf.sqlconnectstr[0]  &&
        (sqlrc = sql_init (sweb_conf.sqlconnectstr, sweb_conf.odbcini,
                           sweb_conf.odbcsysini)) < 0) {
      printf ("Cannot initialize SQL: connect='%s', odbcini='%s', "
              "odbcsysini='%s', rc=%d, uid=%d, euid=%d",
               sweb_conf.sqlconnectstr, sweb_conf.odbcini, sweb_conf.odbcsysini,
               sqlrc, getuid(), geteuid());
      exit(1);
   }

   /*** Initialize the Caucus API for this user. */
   api = a_mak_init ("sweba init");
   chxcpy (api->confid, CQ(sweb_conf.caucus_path));
   sysuserid (empty);
   strcpy (sweb_conf.remote, ascquick(empty));
   strcpy (api->userid, sweb_conf.remote);
   api->terminal = 0;
   api->diskform = sweb_conf.diskform;
   if (a_init (api) != A_OK) {
      fprintf (stderr,
                "Can't a_init.  caucus_path='%s'\n", sweb_conf.caucus_path);
      exit (3);
   }

   syslockfile (sweb_conf.caucus_path, "MISC/lockcml");

   /*** Make sure all required Caucus 5.0 tables exist. */
   if (NOT c5_table_check()) {
      printf ("Cannot access SQL database tables.  Check SWEB/swebd.conf \n"
              "DSN parameters for correct syntax.\n\n");
      logger (1, LOG_FILE, "Cannot create Caucus 5.0 tables!");
      exit   (1);
   }

   /*** Initialize the array of CML lines, and empty VARS and FORMs sets. */
   cfile = make_cml(errtext);
   otab  = make_obtab (100);
   vars  = a_mak_vartab ("sweba vars");
   form  = a_mak_vartab ("sweba form");
   macs  = a_mak_vartab ("sweba macs");
   tg    = target (cd, nullchix);

   strcpy (cmlname, argv[2]);

   /*** Include the globals. */
   cml_globals();

   /*** Load the entire CML file into memory. */
   size = loadcml (cmlname, &sweb_conf, 0, cfile, vars, form,
                   query, macs, otab, tg, nullchix, &found);

   /*** Parse the entire CML file. */
   for (query[0]='\0', num=3;   num<argc;   ++num) {
      if (streq (argv[num], "-l")  &&  num+2 < argc) {
         chxofascii (id_pw, argv[num+1]);
         chxcatval  (id_pw, THIN, L(' '));
         chxcat     (id_pw, CQ(argv[num+2]));
         func_pw    (verified, "pw_verify", id_pw, 1, &sweb_conf);
         if (chxvalue (verified, 0) != '0') {
            a_exit();
            exit (5);
         }
         flag.login = 1;
         strcpy (flag.userid, argv[num+1]);
         strcpy (sweb_conf.remote, argv[num+1]);
         num += 2;
      }
      else {
         strcat (query, argv[num]);
         strcat (query, " ");
      }
   }
   chxclear  (empty);
   parse_cml (tg, cfile, size, 0, &sweb_conf, vars, form,
                  query, &broke, empty, macs, otab, &contd, nullchix);

   /*** This messes things up in BSDI... */
/* buf_close (cd); */

   a_exit();
   sql_done();
   entry_test ("", "");
   exit (0);
}

FUNCTION int is_sweba() {  return (1); }

