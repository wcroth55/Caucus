/*** SWEBD.   Caucus-Web server daemon program.
/
/    Invoked as:
/       swebd   config
/
/    Arguments:
/       config   full pathname of SWEB configuration file.
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
/       Should use something other than perror() to log bugs!
/       WNT40 signal handling is a total guess!
/
/    Home:  swebd.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/27/95 22:22 New program. */
/*: CR  4/15/97 12:36 Integrate with NT. */
/*: CR  5/26/97 17:28 Break subserver off into separate swebs program. */
/*: CR 03/08/00 15:12 Store SOCKET_DELAY value in sockpath/delay. */
/*: CR 03/05/02 13:00 Add robot/pagesucker detection, rejection. */
/*: CR 01/04/04 Ignore child processes earlier (before swebd_script)! */

#include <stdio.h>
#include "chixuse.h"
#include "systype.h"
#include "sweb.h"
#include "swebsock.h"

#include <signal.h>
#include <errno.h>
#include <setjmp.h>

#if UNIX
#include <unistd.h>
#endif

int                 debug = 0;

struct sweb_conf_t  sweb_conf;
static jmp_buf      jmpbuf;


char copyright[] =
"Copyright (C) 2002 CaucusCare.  All rights reserved.";

char *trade[] = {
"This program belongs to CaucusCare.  It is considered a trade secret",
"and is not to be divulged or used by parties who have not received",
"written authorization from the owner."};

int output_default = 0;
int cml_errors     = 0;

FUNCTION  main (int argc, char *argv[])
{
   int    clientlen, child, caucus, root, rc, run, status;
   int    rd, cd, secure;
   int4   now;
   char   temp[512];
   void   sweb_wait(), is_hangup();
   char   this_userid[MAX_USERID+2], prev_userid[MAX_USERID+2];
   int    this_utime, prev_utime;
   FILE  *fp;
#if UNIX
   struct sockaddr_un server, client;
#endif
#if NUT40
   struct sockaddr_in server, client;
#endif
#if WNT40
   SOCKADDR_IN        server, client;
#endif
   ENTRY ("swebd main", "");

   sys_nt();
   signal (SIGINT, SIG_IGN);
   logger_init ("", argv[0], "swd");

   if (argc < 2) {
      logger (1, LOG_ERR | LOG_FILE, "Usage: swebd config_file");
      exit   (1);
   }

   /*** Load the SWEBD configuration file. */
   strcpy       (sweb_conf.filename, argv[1]);
   fix_fullpath (sweb_conf.filename);
   if (NOT load_conf (&sweb_conf)) {
      sprintf (temp, "Cannot read configuration file '%s'.", argv[1]);
      logger  (1, LOG_ERR | LOG_FILE, temp);
      exit    (2);
   }
   logger_init (sweb_conf.caucus_path, argv[0], "swd");
   sprintf (temp, "argv[0]='%s', argv[1]='%s'", argv[0], argv[1]);
   logger  (0, LOG_FILE, temp);
   prev_userid[0] = '\0';
   prev_utime     = time(NULL);


#if UNIX
   /*** Fork a child which will run as a separate process group. 
   /    This process does all the real work; the parent exits immediately. */
   if ( (child = (int) fork()) > 0)  exit (0);
   if (  child < 0) {
      logger (1, LOG_ERR | LOG_FILE, "swebd: cannot fork.");
      exit   (3);
   }

   /*** Make it the process group leader, and disassociate from controlling
   /    terminal, if any. */
   setsid   ();
   signal (SIGCHLD, sweb_wait);    /* Prevent zombie child processes. */
#endif

   /*** Run any "start-up" scripts from swebd. */
   swebd_scripts (&sweb_conf);

   /*** Store SOCKET_DELAY value where swebsock can find it. */
   sprintf   (temp, "%s/SOCKET/delay", sweb_conf.caucus_path);
   sysunlink (temp);
   if (sweb_conf.socket_delay) {
      if ( (fp = fopen (temp, "w")) != NULL) {
         fprintf (fp, "%d\n", sweb_conf.socket_delay);
         fclose  (fp);
      }
   }

   /*** Create RD, a "rendevous" socket, where clients can meet us, and bind
   /    it to a specific file name (Unix) or port # (NT). */
#if UNIX
   rd = socket (AF_UNIX, SOCK_STREAM, 0);
#endif
#if NUT40 | WNT40
   rd = socket (AF_INET, SOCK_STREAM, 0);
#endif
   if (BADSOCK(rd)) {
      sprintf (temp, "swebd: error creating stream socket, errno=%d", errno);
      logger  (1, LOG_FILE, temp);
      exit    (3);
   }

#if UNIX
   server.sun_family = AF_UNIX;
   sprintf (server.sun_path, "%s/SOCKET/sweb", sweb_conf.caucus_path);
#endif
#if WNT40
   memset (&server, 0, sizeof (server));
#endif
#if NUT40 | WNT40
   server.sin_family      = AF_INET;
   server.sin_addr.s_addr = htonl(INADDR_ANY);
   server.sin_port        = htons(SERVER_PORT);
#endif

   status = bind (rd, (SOCKPTR) &server, sizeof(server));
   if (BADSTAT (status)) {
      sprintf (temp, "swebd: error binding socket, path='%s/SOCKET/sweb', "
                     "errno=%d", sweb_conf.caucus_path, errno);
      logger  (1, LOG_FILE, temp);
      exit    (4);
   }


   /*** Put the location of swebfail in a common location. */
   sprintf   (temp, "%s/SOCKET/swebfail", sweb_conf.caucus_path);

#if WNT40
   nt_mkfile (temp, 0755);
#endif
   if ( (fp = fopen (temp, "w")) != NULL) {
      fprintf (fp, "%s/~%s", sweb_conf.hostname, sweb_conf.caucus_name);
      fclose  (fp);
   }
   else {
      sprintf (temp, "swebd: can't write swebfail to '%s'", 
                      sweb_conf.caucus_path);
      logger  (1, LOG_FILE, temp);
      exit    (5);
   }
   
   /*** Tell the kernel we're listening for connections on RD. */
   status = listen (rd, 5);
   if (BADSTAT(status)) {
      logger (1, LOG_FILE, "swebd: error listening");
      exit   (5);
   }

   /*** Process connections on RD as they are made. */
#if UNIX | NUT40
   signal (SIGCHLD, sweb_wait);    /* Prevent zombie child processes. */
   signal (SIGHUP,  is_hangup);    /* Catch hang-ups. */
#endif
#if WNT40
   signal (SIGABRT, is_hangup);    /* just a guess... needs work! */
   signal (SIGTERM, is_hangup);    /* just a guess... needs work! */
#endif
   run = (setjmp (jmpbuf) == 0);
   while (run) {
      logger (0, LOG_FILE, "waiting to accept a connection...");

      /*** New connections create a connection (data) socket CD. */
      clientlen = sizeof(client);
      while (1) {
         cd = accept (rd, (SOCKPTR) &client, &clientlen);
         if (NOT BADSOCK(cd))                      break;
         if (    BADSOCK(cd)  &&  errno != EINTR)  break;
      }
      logger_init (sweb_conf.caucus_path, argv[0], "swd");

      if (BADSOCK(cd)) {
         logger (1, LOG_FILE, "swebd: error accepting connection");
         continue;
      }

      now = time(NULL);

      /*** If we have a request for another swebs, from the same userid,
      /    within X seconds, reject -- probably a robot or pagesucker. */
#if UNIX | NUT40
      read (cd, this_userid, MAX_USERID);    this_userid[MAX_USERID] = '\0';
#endif
      if (NOT NULLSTR(this_userid)  &&  streq (this_userid, prev_userid)  &&
          now - prev_utime < sweb_conf.robot_reject) {
         child  = -7;   /* reject this request. */
         secure = sweb_conf.robot_reject;
      }

      /*** Otherwise, spawn off a child "subserver" to actually deal with 
      /    requests made from that client, and pass it back. */
      else {
         srand (now);
         secure = rand() % L(1000000);
         child  = sys_subserver (&sweb_conf, secure, cd);
         sprintf (temp, "swebs child = %d", child);
         logger  (0, LOG_FILE, temp);
         if (child < 0) {
            logger (1, LOG_FILE, "swebd: can't create child");
   /*       close  (cd);
            cd = -1;
            continue; */
         }
         strcpy (prev_userid, this_userid);
         prev_utime = now;
      }

      /*** Notify the prospective client who the child is. */
      sprintf (temp, "%07d%07d", child, secure);

#if UNIX | NUT40
      write   (cd, temp, 15);
      close   (cd);
#endif
#if WNT40
      send (cd, temp, 15, 0);
      closesocket (cd);
#endif
      cd = INV_SOCK;
   }

#if UNIX | NUT40
   if (NOT BADSOCK(cd))    close (cd);
   close   (rd);
#endif

#if WNT40
   if (NOT BADSOCK(cd))    closesocket (cd);
   closesocket (rd);
#endif

#if UNIX
   sprintf (temp, "%s/SOCKET/sweb", sweb_conf.caucus_path);
   unlink  (temp);
#endif

   exit    (0);
}


FUNCTION  void sweb_wait()
{
#if UNIX | NUT40
   wait   (0);
   signal (SIGCHLD, sweb_wait);
#endif
}


FUNCTION  void is_hangup (int value)
{
   longjmp (jmpbuf, 1);
}
