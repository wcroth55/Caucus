
/*** SWEBS.   Caucus-Web subserver.
/
/    Invoked as:
/       swebs   config   secure
/
/    Arguments:
/       config   full pathname of SWEB configuration file.
/       secure   security code for this subserver.
/
/    Purpose:
/
/    How it works:
/
/    Exit status: 0 on success, 1 on failure.
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  sweb/swebs.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/26/97 17:08 New program, derived from 3.1 swebd subserver. */
/*: CR 03/08/00 15:12 Test for comm err in reading from swebsock client. */
/*: CR 10/07/01 15:00 Raise userid length. */
/*: CR  4/02/02 12:40 Cookie login support. */
/*: CR  3/21/03 Add is_sweba() function. */
/*: CR  5/23/03 Add sql... features. */
/*: CR 11/12/03 Add entry_test() call to force proper linking. */
/*: CR 01/30/05 Add authmethod 3 (md5 hash). */
/*: CR 02/15/05 Handle swebd.conf stderr parameter. */
/*: CR 02/21/05 Change to stderrTo, call sysmd5(). */
/*: CR 04/06/05 stderrTo "off" must go to /dev/null */
/*: CR 02/12/07 Run timeout.cml on receiving SIGHUP(kill) or SIGALRM(timeout) */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "sweb.h"
#include "systype.h"
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>

#if UNIX
#include <unistd.h>
#endif

int                 cml_debug      = 0;
int                 output_default = 0;
int                 cml_errors     = 0;
struct sweb_conf_t  conf;

extern Vartab   vars, form, macs;

#define  BUFMAX 30000
AsciiBuf sqlbuf;
int client_pid;

static jmp_buf      jmpbuf;

char copyright[] =
"Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved. "
"This program is published under the terms of the GPL; see "
"the file license.txt in the same directory.";

FUNCTION  main (int argc, char *argv[])
{
   int     clientlen, child, caucus, root, run;
   Hose    cd, hose_accept();
   char    temp[1024], remote[MAX_USERID+2], ip_addr[32], tmpdir[128];
   char    caucus_ver[100];
   int     first_service = 1;
   int     subcode, error, pagetime, badcode, errcomm;
   int     lice_code = 0;
   int     count = 0;
   int     locksetup = 0;
   int     sqlrc, badauth3, plus;
   int4    kbytes, files;
   Initapi api = (Initapi) NULL;
   Chix    ctemp;
   void    sub_shutdown();
   void    jumper();
   char   *mytime(), *strplain();
   AsciiBuf ascii_buf();

   Target   tg, target();
   CML_File cfile;
   char     timeoutName[] = "timeout.cml";
   char     query[] = "";
   int      timeoutFound, size, broke, contd;
   Obtab    otab;
   Chix     timeoutResult, empty;
   CML_File make_cml();
   Obtab    make_obtab();
   char     reasonDied[200];

   ENTRY ("swebs main", "");

   sys_nt();
   signal (SIGINT, SIG_IGN);
   logger_init ("", argv[0], "sws");
   sqlbuf = ascii_buf (NULL, BUFMAX);

   if (argc < 3) {
/*    logger (1, LOG_FILE, "Usage: swebs config_file securecode"); */
      display_license();
      exit   (1);
   }

   /*** Load the SWEBD configuration file. */
   strcpy (conf.filename, argv[1]);
   if (NOT load_conf (&conf)) {
      sprintf (temp, "Cannot read configuration file '%s'.", argv[1]);
      logger  (1, LOG_FILE, temp);
      exit    (2);
   }

   /*** Handle stderr parameter */
   if      (streq  (conf.stderrTo, "on")) ;
   else if (streq  (conf.stderrTo, "off")  ||
            NULLSTR(conf.stderrTo))  freopen ("/dev/null",   "w", stderr);
   else                              freopen (conf.stderrTo, "a", stderr);

   logger_init (conf.caucus_path, argv[0], "sws");
   sscanf (argv[2], "%d", &conf.secure);
   conf.secure = conf.secure ^ 0xAA55;

   /*** Log subserver start-up. */
#if UNIX | NUT40
   sprintf (temp, "\n"
     "    Request   Actual\n"
     "  euid: %3d   %3d\n"
     "  ruid: %3d   %3d\n"
     "  egid: %3d   %3d\n"
     "  rgid: %3d   %3d\n",
     conf.caucus_id,    (int) geteuid(), conf.real_id,      (int) getuid(),
     conf.caucus_group, (int) getegid(), conf.real_group,   (int) getgid());
   logger (0, LOG_FILE, temp);
#endif

   /*** Subserver timeout (or a HANGUP from an external source) should
   /    trigger 'jumper', which will make us exit cleanly. */
#if UNIX | NUT40
   signal (SIGHUP,  jumper);
   signal (SIGALRM, jumper);
#endif

#if UNIX | NUT40
   /*** Ignore "broken" pipe signal from users too quick to click. */
   signal (SIGPIPE, SIG_IGN);

   /*** Ignore "child died" signal (created by use of $asynch() function). */
   signal (SIGCHLD, SIG_IGN);
#endif


   /*** Check for minimum amount of disk space; if too small,
   /    encode in lice_code. */
   if (sysavail (conf.caucus_path, &kbytes, &files, &error)) {
      if (kbytes < MIN_DISK_SPACE  ||
          (files < MIN_DISK_FILES  &&  files != -1)) lice_code = LICE_NODISK;
   }

   /*** Requires MySQL connection. */
   if (NULLSTR(conf.sqlconnectstr)  ||
         (sqlrc = sql_init (conf.sqlconnectstr, conf.odbcini,
                              conf.odbcsysini)) < 0) {
         sprintf (temp, "Cannot initialize SQL: connect='%s', odbcini='%s', "
                        "odbcsysini='%s', rc=%d, uid=%d, euid=%d",
                  conf.sqlconnectstr, conf.odbcini, conf.odbcsysini,
                  sqlrc, getuid(), geteuid());
         logger  ( 1, LOG_FILE, temp);
         exit (2);
   }

   /*** Make sure all required Caucus 5.0 tables exist. */
   if (NOT c5_table_check()) {
      logger (1, LOG_FILE, "Cannot create Caucus 5.0 tables!");
      exit   (1);
   }

   /*** Process each connection and request from the browser client,
   /    until we get a timeout or HANGUP. */
   strcpy (reasonDied, "died-setjmp");
   if (setjmp (jmpbuf) == 0)  while (1) {

      /*** Wait for a client to request to talk to us, within
      /    conf.timeout seconds. */
      if ( (cd = hose_accept (&conf, conf.timeout)) == BADHOSE)  {
         sprintf (reasonDied, "NEW7: died-timeout %d", conf.timeout);
         break;
      }
  
      /*** Get the client's security code, userid, and ip address. */
      remote[0] = 0xFF;
      hose_read (cd, temp,     7, 0);             temp   [ 7] = '\0';
      hose_read (cd, remote,  MAX_USERID, 0);     remote [MAX_USERID] = '\0';
/*    hose_read (cd, &client_pid, sizeof(int), 0);   RA2 */
      hose_read (cd, &conf.auth_method, 1, 0);
      hose_read (cd, caucus_ver, 10, 0);          caucus_ver[10] = '\0';
      hose_read (cd, ip_addr, 25, 0);             ip_addr[25]    = '\0';
/*    { char x[1000];
        sprintf (x, "userid='%s', auth=%d, ver='%s', ip='%s'",
                     remote, conf.auth_method, caucus_ver, ip_addr);
        logger (1, LOG_FILE, x);
      }
*/

      /*** Auth method 3 means user's browser had a CAUCUSID cookie,
      /    in the format "userid+hash", where "hash" is the MD5
      /    encryption of the string "userid,tag".  The value of
      /    tag is in turn taken from the swebd.conf configuration
      /    file parameter "auth3tag".  */
      badauth3 = 0;
      if (conf.auth_method == 3) {
         if ( (plus = strindex (remote, "+")) > 0) {
            char          md5hash[100], hashit[MAX_USERID];
            int           i;

            strcpy   (md5hash, remote+plus+1);
            strlower (md5hash);
            remote[plus] = '\0';
            strcpy (hashit, remote);
            strcat (hashit, ",");
            strcat (hashit, conf.auth3tag);

            sysmd5   (hashit, hashit);
            strlower (hashit);
            badauth3 = (streq (hashit, md5hash) ? 0 : 1);
            if (badauth3) {
               sprintf (temp, "hashit =%s\nmd5hash=%s\n", hashit, md5hash);
               logger (1, LOG_FILE, temp);
            }
         }
      }

      sscanf (temp, "%7d", &subcode);
      if (first_service) {
         strcpy (conf.remote,     remote);
         strcpy (conf.ip_addr,    ip_addr);
         strcpy (conf.caucus_ver, caucus_ver);
      }
      else if (NULLSTR(conf.remote)  &&  NOT NULLSTR(remote)) {
         strcpy (conf.remote,     remote);
         strcpy (conf.caucus_ver, caucus_ver);
         strcpy (api->userid, conf.remote);
         a_init (api);
         set_laston();
      }


      /*** If this is an illegal attempt to communicate (bad userid, bad
      /    security code, or comm error), complain and don't serve anything.*/
      badcode = conf.secure != subcode                  ||
                NOT streq (remote,     conf.remote)     ||
                badauth3                                ||
               (NOT NULLSTR(remote)  &&
                NOT streq (caucus_ver, conf.caucus_ver));
      errcomm = remote[0] < 0           ||  (remote[0] & 0xFF) == 0xFF;
      if (badcode || errcomm) {
         hose_write (cd, "NO", 3);
         hose_close (cd);
         sprintf (temp,
          "BAD: remote='%s', userid='%s', secure=%07d, subcode=%07d, ver='%s'",
                     conf.remote, remote, conf.secure, subcode, caucus_ver);
         logger (0, LOG_FILE, temp);
         if (errcomm)  {
            strcpy (reasonDied, "NEW5: died-errcom");
            break;        /* Something's hosed, exit. */
         }
         if (badcode)  {
            strcpy (reasonDied, "NEW6: died-badcode");
            break;
         }
      }

      if (first_service) {
         sprintf (temp, "connect (%s)...", conf.remote);
         logger (0, LOG_FILE, temp);

         /*** Initialize the Caucus API for this user. */
         if (api == (Initapi) NULL)  api = a_mak_init ("subserver init");
         chxcpy (api->confid, CQ(conf.caucus_path));
         strcpy (api->userid, conf.remote);
         api->terminal = 0;
         api->diskform = conf.diskform;
         if (a_init (api) != A_OK) {
            logger (1, LOG_FILE, "a_init() failed");
            sprintf (temp,
                 "Can't a_init.  caucus_path='%s'\n", conf.caucus_path);
            logger (1, LOG_FILE, temp);
            sub_shutdown(-1);
         }
         set_laston();
         first_service = 0;
         if (NOT locksetup)  syslockfile (conf.caucus_path, "MISC/lockcml");
         locksetup = 1;
      }


      /*** This is a valid userid & request, go ahead and process it. */
      hose_write (cd, "OK", 3);
      pagetime = conf.pagetime;
      hose_write (cd, (char *) &pagetime, sizeof(int));
      process_request (cd, &conf, lice_code, ip_addr);

      buf_close (cd);
   }

   logger (0, LOG_FILE, reasonDied);

   /*** Here if we received a SIGHUP or SIGALRM (kill or a timeout).
   /    Run the timeout.cml to do any UI level cleanup. */
   ctemp = chxalloc (16L, THIN, "swebs ctemp");
   chxofascii (ctemp, "Cannot load timeout.cml");
   cfile = make_cml(ctemp);
   otab  = make_obtab (100);
   timeoutResult = chxalloc (16L, THIN, "swebs timeoutResult");
   empty = chxalloc (16L, THIN, "swebs empty");
   tg = target (0, timeoutResult);

   size = loadcml (timeoutName, &conf, 0, cfile, vars, form,
                   query, macs, otab, tg, nullchix, &timeoutFound);
   parse_cml (tg, cfile, size, 0, &conf, vars, form,
                  query, &broke, empty, macs, otab, &contd, nullchix);

   chxofascii    (ctemp, "logout");
   func_logevent (ctemp);
   logger (0, LOG_FILE, "shutting down!");
   sub_shutdown(23);
}


FUNCTION  void jumper (int value)
{
   longjmp (jmpbuf, 1);
}


/*** Controlled shutdown of subserver. */
FUNCTION  void sub_shutdown (int value)
{
   char   temp[200];
   char  *mytime();

   logger (0, LOG_FILE, "in sub_shutdown");

   /*** Update time last on for this user. */
   set_laston();

   /*** Delete any temporary files created by file uploads. */
   form_temp (0);

#if UNIX | NUT40
   sprintf (temp, "%s/SOCKET/sweb%06d", conf.caucus_path, (int) getpid());
   unlink  (temp);
#endif
   a_exit  ();

   /*** Log subserver shut-down. */
   sprintf (temp, "stop:  %s, signal=%d\n", mytime(), value);
   logger  (0, LOG_FILE, temp);

   sql_done();
   entry_test ("", "");
   exit    (0);
}

FUNCTION int is_sweba() {  return (0); }
