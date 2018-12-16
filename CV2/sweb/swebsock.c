
/*** SWEBSOCK.  Send a browser request to a swebd sub-server, including
/    (possibly) an initial request to assign a sub-server.
/
/    Invoked as:
/       swebsock
/
/    Arguments: (supplied as environment variables)
/       PATH_INFO cases:
/          goodpid/code/SP/file.cml...    (send request for file.cml)
/          deadpid/code/SP/file.cml...    (dead subserver)
/          0/0/SP/file.cml                (get me new subserver)
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
/    Home:  swebsock.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/27/96 13:59 New program, merging on old swebcli & swebtalk. */
/*: CR  7/19/99 14:07 More reporting if bad socketpath. */
/*: CR 03/08/00 15:12 Use SOCKET_DELAY value if any; do 3 retries. */
/*: CR 04/27/00 13:18 Add HTTP_ACCEPT, expand sizes to 200 chars. */
/*: CR 10/07/01 15:00 Raise userid length. */
/*: CR  3/05/02 12:50 Add userid arg to new_subserver() call. */
/*: CR  4/02/02 12:40 Cookie login support. */
/*: CR  8/16/02 Treat rejection as comm failure, gets nouserid page in end */
/*: CR  8/21/02 Make cookie names depend on swebs process id! */
/*: CR  4/23/03 If REQUEST_URI is empty set it to SCRIPT_NAME + PATH_INFO. */
/*: CR  5/04/03 Allow max size of REQUEST_URI = 2000 */
/*: CR  4/19/04 REQUEST_URI needs QUERY_STR as well! */
/*: CR  7/05/04 Strip any embedded blanks from userid. */
/*: CR 01/25/05 Add auth_method 3, cookie called CAUCUSID. */
/*: CR 02/09/10 Add removeOpenidProtocol() to turn an OpenId into userid. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include <sys/types.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>

#if UNIX
#include <unistd.h>
#endif

#if UNIX
#include <sys/socket.h>
#include <sys/un.h>
#endif

#if WIN32
#include <fcntl.h>
#include <io.h>
#endif

#define  MAX_STALL   10   /* # of retries for a "stalled" subserver. */
#define  MAX_TEMP  2000

static unsigned char buffer[PIPEBUF+2];
static          char sockpath[200];
static          int  subpid;
static jmp_buf       jmpbuf;

int                  debug = 0;

FUNCTION  main (argc, argv)
   int    argc;
   char  *argv[];
{
   struct sockmsg_t   sockmsg;
   Hose   sockfd;
   int    subcode, dataread, got, amount, attempts, rc;
   int    acksent, bp, pagetime, socket_delay, tries;
   int    cu, cv;
   char   auth_method;
   int4   content_length, urilen;
   char   remote[100], pid[20], secure[20], content[200];
   char   temp[MAX_TEMP], userid[MAX_USERID+2], pathinfo[300], cml[200], 
          tmpdir[200], caucus_ver[100], request_uri[MAX_TEMP],
          querystr[MAX_TEMP];
   char   verpid[100], usrpid[100], textlen[80];
   char   ack;
   char  *t, *u;
   char  *socket_path();
   char  *mytime();
   FILE  *fp;
   void   swebt_shutdown (int value);
   void   jumper();
   void   removeOpenidProtocol ();

#if WIN32
   /*** Make sure we read forms, etc. input from the web server in
   /    binary mode, else the CONTENT_LENGTH count will be off! */
   _setmode (0, _O_BINARY);
#endif

   /*** ==============================================================
   /    SECTION A: get SOCKPATH and the subserver pid.  */

   /*** Determine the location of SOCKPATH, the directory where the
   /    sockets live. */
   logger_init ("", argv[0], "soc");
   systmpdir (tmpdir, 0);
   strcpy (sockpath, socket_path ("", argv[0]));
   if (NULLSTR(sockpath)) {
      printf ("Content-type: text/plain\n\n\n");
      printf ("Cannot read socket path from swebsock!\n");
      fflush (stdout);
#if UNIX
      system ("set");
#endif
      exit   (0);
   }

   /*** If SOCKPATH is incorrect, log fact and exit. */
   sprintf (temp, "%s/swebfail", sockpath);
   if ( (fp = fopen (temp, "r")) != NULL)  fclose (fp);
   else {
      printf ("Content-type: text/plain\n\n\n");
      printf ("swebfail='%s'\n", temp);
      printf ("errno=%d\n", errno);
      printf ("Hint: If file is correct, volume may not be mounted suid.\n");
      sprintf (temp, 
         "bad sockpath!\npid=%6d, time=%s, sockpath='%s', argv[0]='%s'\n", 
                       (int) getpid(), mytime(), sockpath, argv[0]);
      printf ("Cannot read socket path from swebsock!\n");
      printf ("%s\n", temp);
      fflush (stdout);
      logger (1, LOG_FILE, temp);
#if UNIX
      system ("set");
#endif
      exit   (0);
   }


   /*** Verify that swebsock was started. */
   sprintf (temp,  "pid=%6d, time=%s, sockpath='%s'", 
                    (int) getpid(), mytime(), sockpath);
   logger (0, LOG_FILE, temp);

   /*** Determine the userid and the authentication method. */
   auth_method = 1;
   caucus_ver[0] = '\0';
   env_var  (userid, "REMOTE_USER");
   removeOpenidProtocol (userid);
   if (NOT userid[0])  env_var (userid, "AUTH_USER");
   env_var  (pathinfo, "PATH_INFO");

   /*** Get the subserver connection information (and the requested CML
   /    script) out of PATH_INFO. */
   if (NULLSTR(pathinfo))  {
      printf ("Content-type: text/plain\n\n");
      sprintf (temp, "No PATH_INFO!\n"
                     "pid=%6d, time=%s, userid=%s No Path Info!\n",
                      (int) getpid(), mytime(), userid);
      logger (1, LOG_FILE | LOG_OUT, temp);
      exit   (1);
   }
   sscanf  (pathinfo, "/%d/%d/%s", &subpid, &subcode, cml);

   /*** Cookie login/authentication support; get userid and verification code.*/
   if (NOT userid[0]) {
      env_var (temp, "HTTP_COOKIE");
      sprintf (usrpid, "caucus_usr%d=", subpid);
      sprintf (verpid, "caucus_ver%d=", subpid);
      cu = strindex (temp, usrpid);
      cv = strindex (temp, verpid);
      if (cu >= 0  &&  cv >= 0) {
         strcpytil (userid,     temp+cu+strlen(usrpid), ';');
         strcpytil (caucus_ver, temp+cv+strlen(verpid), ';');
         auth_method = 2;
      }
      else if ( (cu = strindex (temp, "CAUCUSID=")) >= 0) {
         strcpytil   (userid, temp+cu+9, ';');
         strunescape (userid);
         if (userid[0]) {
            strcpy    (caucus_ver, "auth3");
            auth_method = 3;
         }
      }
      
   }

   /*** Remove any embedded blanks from userid. */
   strcpy (temp, userid);
   for (t=temp, u=userid;   *t;   ++t) {
      if (*t != ' '  &&  *t != '\t')  *u++ = *t;
   }
   *u = '\0';
   if (NOT userid[0])  auth_method = 0;


   /*** ==============================================================
   /    SECTION B: connect to the subserver. */

   /*** If we've got a desired subserver, try to "shake hands" with it. */
   if (subpid > 0) {
      rc = shake_subserver (sockpath, subpid, subcode, userid, 
                            caucus_ver, auth_method, &sockfd);
      if (rc <  0)  subpid =  0;    /* comm failure, try a new subserver. */
/*    if (rc == 0)  subpid = -8; */ /* outright rejection */
      if (rc == 0)  subpid =  0;    /* Treat rejection as com failure,
                                    /  user gets 'nouserid' page in the end */
   }
      
   /*** If we're looking for a new subserver (or the handshake failed),
   /    request a new subserver and try to "shake hands" with *it*. */
   if (subpid == 0) {

      /*** See if swebd wrote an initial socket delay parameter for us. */
      socket_delay = 0;
      sprintf (temp, "%s/delay", sockpath);
      if ( (fp = fopen (temp, "r")) != NULL) {
         fscanf (fp, "%d", &socket_delay);
         fclose (fp);
      }

      /*** Request a new subserver, and then attempt to connect to
      /    (handshake with) it.  Use the initial socket delay value
      /    (if any) between the request and the handshake.  If the
      /    handshake fails, retry the sequence with a (larger) delay. */
      for (tries=0;   tries < 4;   ++tries) {
         new_subserver (sockpath, &subpid, &subcode, userid);
         if (subpid <= 0)  break;   /* failed altogether. */

         if (socket_delay)  sleep (socket_delay);
         rc = shake_subserver (sockpath, subpid, subcode, userid, 
                               caucus_ver, auth_method, &sockfd);
         if (rc != -1)  break;
         socket_delay += 4;
      }
   }

   if (subpid == -7) {
      printf ("Content-type: text/plain\n\n");
      printf ("Automatic robot reject.  "
              "Try again or press reload in %d seconds.\n", subcode);
      exit   (1);
   }

   if (subpid == -8) {
      printf ("Content-type: text/plain\n\n");
      printf ("Authentication error.  Please start over.\n");
      env_var (temp, "REQUEST_URI");       temp[MAX_TEMP-1] = '\0';
      printf ("request_uri='%s'\n", temp);
      exit   (1);
   }

   if (subpid < 0) {
      printf ("Content-type: text/plain\n\n");
      sprintf (temp, "Could not start swebs!\n  time=%s, userid=%s",
                      mytime(), userid);
      logger  (1, LOG_FILE | LOG_OUT, temp);
      exit    (1);
   }

   /*** If the handshake failed, give up. */
   if (sockfd == BADHOSE) {
      printf ("Content-type: text/plain\n\n");
      sprintf (temp, 
         "Handshake failed!\n  pid=%6d, time=%s, userid=%s, tries=%d",
                      (int) getpid(), mytime(), userid, tries);
      logger  (1, LOG_FILE | LOG_OUT, temp);
      exit    (1);
   }

   if (tries > 0) {
      sprintf (temp, "delayed handshake worked, tries=%d", tries+1);
      logger  (0, LOG_FILE, temp);
   }
      
   /*** Read the "timeout" value for swebsock from swebs! */
   got = hose_read (sockfd, (char *) &pagetime, sizeof(pagetime), 120);
   logRead (subpid, sockfd, sizeof(pagetime), got);

   /*** ==============================================================
   /    SECTION C:  Write data to subserver, and echo results to stdout. */

   /*** Decode the POST'ed content length, if any. */
   env_var (content, "CONTENT_LENGTH");
   if (NULLSTR(content))   content_length = 0;
   else                    sscanf (content, "%d", &content_length);

   /*** Tell the subserver which CML file we want, and the value
   /    of the arguments (if any). */
   cml [199] = '\0';
   hose_write (sockfd, cml,  200);

   /*** Pass on requested CGI environment variables. */
   env_var (temp, "QUERY_STRING");      temp[199] = '\0';
   hose_write (sockfd, temp, 200);
   env_var (temp, "HTTP_USER_AGENT");   temp[199] = '\0';
   hose_write (sockfd, temp, 200);
   env_var (temp, "HTTP_REFERER");      temp[199] = '\0';
   hose_write (sockfd, temp, 200);
   env_var (temp, "HTTP_ACCEPT");       temp[199] = '\0';
   hose_write (sockfd, temp, 200);
   env_var (temp, "REQUEST_URI");       temp[MAX_TEMP-1] = '\0';
   if (NULLSTR(temp)) {
      env_var (temp, "SCRIPT_NAME");
      env_var (request_uri, "PATH_INFO");
      env_var (querystr,    "QUERY_STRING");
      strcat  (temp, request_uri);
      strcat  (temp, "?");
      strcat  (temp, querystr);
      temp[MAX_TEMP-1] = '\0';
   }
   urilen = strlen(temp)+1;
   sprintf (textlen, "%05d", urilen);
   hose_write (sockfd, textlen, 6);
   hose_write (sockfd, temp, urilen);

   /*** Read any data POST'ed by a form, from stdin, and write it
   /    to the subserver. */
   sprintf (temp, "%07d", content_length);
   hose_write (sockfd, temp, 8);
   for (dataread=0;   dataread < content_length;   dataread += got) {
      amount = content_length - dataread;
      if (amount > PIPEREAD)  amount = PIPEREAD;
#if UNIX | NUT40
      got =  read (0, buffer, amount);
#endif
#if WIN32
      got = _read (0, buffer, amount);
#endif
      /*** If we've run out of data, but CONTENT_LENGTH is not
      /    satisfied, pad with zeroes.   (Shouldn't happen, but
      /    one never knows... */
      if (got <= 0) {
         got = amount;
         for (bp=0;   bp<got;   ++bp)  buffer[bp] = '\0';
      }

      buffer[got] = '\0';
      pipewrite (sockfd, (char *) buffer, got, PIPEWRITE);
   }

   /*** Read subserver's response, and echo it back out to stdout, until
   /    there's no more.  If the subserver appears to "stall", sleep
   /    for a bit and try again up to MAX_STALL times. (That last
   /    sentence no longer makes sense... ???) */
#if UNIX | NUT40
   signal (SIGHUP,  jumper);
   signal (SIGALRM, jumper);
#endif
   if (setjmp (jmpbuf) != 0)  {
      myLogS("shutdown", "");
      swebt_shutdown(0);
   }
   
   attempts = 0;   
   ack = 0xFF;
   myLogN("pagetime", pagetime);
   do {
#if UNIX | NUT40
      alarm (pagetime);  
#endif

      got = hose_read (sockfd, (char *) &sockmsg, sizeof(sockmsg), pagetime);
      logRead (subpid, sockfd, sizeof(sockmsg), got);

#if LNX12
      acksent = send (sockfd, &ack, 1, 0);
      logSend (subpid, acksent);
#endif

#if UNIX | NUT40
      alarm (  0);
#endif
      myLogN("got", got);
      myLogN("acksent", acksent);

      if (got <= 0)  {
         if (++attempts > MAX_STALL)  break;
         syssleep (5);
         continue;
      }

      myLogN("sockmsg.len", sockmsg.len);
      sockmsg.buf[sockmsg.len] = '\0';
      fputs (sockmsg.buf, stdout);
      fflush (stdout);

/*    syssleep(3);  */   /* Simulate slow client. */

   } while (NOT sockmsg.eof);

   hose_close (sockfd);

   /*** Log that we're done. */
   sprintf (temp,  "      DONE: time=%s\n\n", mytime());
   logger  (0, LOG_FILE, temp);
   exit    (0);
}


/*** Do a controlled shutdown and cleanup on receipt of signals. */

FUNCTION  void swebt_shutdown (int value)
{
   logger (0, LOG_FILE, "Alarm shutdown");
   exit   (0);
}


FUNCTION  void jumper (int value)
{
   longjmp (jmpbuf, 1);
}

FUNCTION  void removeOpenidProtocol (char *userid) {
   int lastPos;
   if (strindex (userid, "http://") == 0) {
      stralter  (userid, "http://", "");
      lastPos = strlen(userid) - 1;
      if (userid[lastPos] == '/')  userid[lastPos] = '\0';
   }
}

FUNCTION void myLogN(char *msg, int number) {
#if DIAGNOSE
   char temp[500];
   sprintf (temp, "NEW1: %s=%d\n", msg, number);
   logger (1, LOG_FILE, temp);
#endif
}

FUNCTION void myLogS(char *msg, char text) {
#if DIAGNOSE
   char temp[500];
   sprintf (temp, "NEW2: %s=%s\n", msg, text);
   logger (1, LOG_FILE, temp);
#endif
}

FUNCTION logRead(int subpid, int sockfd, int size, int got) {
#ifDIAGNOSE
   char temp[200];
   char *prefix;

   prefix = " |||";
   if (got <= 0)  prefix = " <<<";
   sprintf (temp, "NEW3: %s read from pid=%d on fd=%d size=%d got=%d, errno=%d", 
         prefix, subpid, sockfd, size, got, errno);
   logger (1, LOG_FILE, temp);
#endif
}

FUNCTION logSend(int subpid, int acksent) {
#if DIAGNOSE
   char temp[200];
   char *prefix;
 
   prefix = " |||";
   if (acksent <= 0)  prefix = " <<<";
   sprintf (temp, "NEW4: ||| send to pid=%d, size=1, ack=%d, errno=%d", subpid, acksent, errno);
   logger (1, LOG_FILE, temp);
#endif
}
