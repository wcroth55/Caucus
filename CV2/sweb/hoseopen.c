
/*** HOSE_OPEN.  Try to open a connection with a hose already prepared by
/                a subserver.
/
/    fd = hose_open (sockpath, subpid, userid);
/   
/    Parameters:
/       Hose   fd;          (returned hose)
/       char  *sockpath;    (path to SOCKET directory)
/       int    subpid;      (pid of subserver we wish to connect to)
/       char  *userid;      (browser user's userid -- for logging)
/
/    Purpose:
/
/    How it works:
/
/    Returns: 
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/hoseopen.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/15/97 13:59 New function. */

#include <stdio.h>
#include "systype.h"
#include "chixuse.h"
#include "sweb.h"

#if UNIX | NUT40
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <errno.h>
#endif

#if NUT40 | WNT40
#include <setjmp.h>
#include <fcntl.h>
static jmp_buf jmpbuf;
#endif

#if WNT40
#include <windows.h>
#include <iostream.h>
#include <string.h>
#include <stddef.h>
#include <process.h>
static jmp_buf jmpbuf;
#endif

FUNCTION  Hose hose_open (
   char  *sockpath,
   int    subpid,
   char  *userid )
{

#if UNIX
   struct sockaddr_un subserv;
   int    attempts, sockfd, success;
   char   temp[256];
   char  *mytime();

   /*** Create a socket, and connect it to the "file" that the subserver
   /    is listening on.  Log or complain as appropriate. */
   for (attempts=0;   attempts <= MAX_ATTEMPT;   ++attempts) {
      sockfd = socket (AF_UNIX, SOCK_STREAM, 0);
      if (sockfd >= 0)  break;
      syssleep (2);
   }

   sprintf (temp, "hoseopen: pid=%6d, subpid=%6d, time=%s, sockfd=%d",
                   (int) getpid(), subpid, mytime(), sockfd);
   logger  (0, LOG_FILE, temp);

   if (sockfd < 0) {
      sprintf (temp, "%s, errno=%d",
             "Swebsock (shake_subserver) could not create stream socket",
             errno);
      logger (1, LOG_FILE, temp);
      exit   (1);
   }

   /*** OK, we've got a socket, now try to connect through it to 
   /    our (presumed) subserver.   Try several times. */
   subserv.sun_family = AF_UNIX;
   sprintf (subserv.sun_path, "%s/sweb%06d", sockpath, subpid);

   for (attempts=0;   attempts <= MAX_ATTEMPT;   ++attempts) {
      success = (connect (sockfd, (struct sockaddr *) &subserv,
                          sizeof(subserv)) == 0);
      sprintf (temp, 
         "pid=%6d, subpid=%6d, time=%s, userid=%s\n errno=%d",
          (int) getpid(), subpid, mytime(), userid, 
          (success ? 0 : errno));
      logger (0, LOG_FILE, temp);
      if (success)     break;

      syssleep (3);
   }

   /*** If we can't connect, the subserver probably isn't there (is dead). */
   if (NOT success) {
      close  (sockfd);
      return (BADHOSE);
   }

   return (sockfd);
}
#endif


#if NUT40
   int    hd, success, attempts;
   char   pipename[256];
   void   hose_fail();
   char   temp[512];

   /*** Open the named pipe to the specified subserver.  (It might take
   /    several tries, in case the subserver is still closing up the
   /    last request.) */
   sprintf (pipename, "//./pipe/sweb%06d", subpid);
   for (attempts=0;   attempts<MAX_ATTEMPT;   ++attempts) {
      if ( (hd = open (pipename, O_BINARY | O_RDWR)) >= 0)  break;
      syssleep (2);
   }
   if (hd < 0)  {
      sprintf (temp, "hoseopen failed, pipename='%s', errno=%d",
                     pipename, errno);
      logger  (1, LOG_FILE, temp);
      exit    (1);
   }

   /*** Write its pid to it, and wait up to 20 seconds for an answer. */
   if (setjmp (jmpbuf) != 0) {
      close  (hd);
      return (BADHOSE);
   }
   signal (SIGALRM, hose_fail);
   alarm  (20);
   write  (hd, &subpid, sizeof(subpid));

   success = 0;
   read (hd, &success, sizeof(success));
   alarm  ( 0);

   if (success != subpid+1) {
      close  (hd);
      return (BADHOSE);
   }

   return (hd);
}

FUNCTION  void hose_fail (int value)
{
   longjmp (jmpbuf, 1);
}
#endif


#if WNT40
   Hose   hd;
   char   pipename[256];
   int    success, attempts, stat;

   void   hose_fail();
   char   temp[512];
   int4   ec;
   SECURITY_DESCRIPTOR sd;
   SECURITY_ATTRIBUTES sa;

   InitializeSecurityDescriptor (&sd, SECURITY_DESCRIPTOR_REVISION);
   SetSecurityDescriptorDacl (&sd, TRUE, NULL, FALSE);
   sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   sa.bInheritHandle = FALSE;
   sa.lpSecurityDescriptor = &sd;

   /*** Open the named pipe to the specified subserver.  (It might take
   /    several tries, in case the subserver is still closing up the
   /    last request.) */
   sprintf (pipename, "\\\\.\\pipe\\sweb%06d", subpid);
   for (attempts=0;   attempts<MAX_ATTEMPT;   ++attempts) {
      hd = CreateFile (pipename, GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_READ, &sa, OPEN_EXISTING,
               FILE_ATTRIBUTE_NORMAL
#if USE_OVERLAP
               | FILE_FLAG_OVERLAPPED
#endif
               ,
               (HANDLE) NULL);
      ec = GetLastError();
      if (hd != INVALID_HANDLE_VALUE)  break;
      syssleep (2);
   }
   if (hd == INVALID_HANDLE_VALUE) {
      sprintf (temp, "invalid file handle from opening pipe, error=%d", ec);
      logger  (0, LOG_FILE, temp);
      return  (BADHOSE);
   }

   /*** Write its pid to it, and wait up to 20 seconds for an answer. */
   hose_write (hd, &subpid, sizeof(subpid));
   success = 0;
   stat = hose_read  (hd, &success, sizeof(success), 20);

   if (stat < 0  ||  success != subpid+1) {
      logger (1, LOG_FILE, "hose_read failed to get subpid+1");

      /*** In theory, we should do a "hose_close(hd);" here.  But it
      /    makes swebsock abort!  ARGGHHHH....! */
      return (BADHOSE);
   }

   return (hd);
}

FUNCTION  void hose_fail (int value)
{
   longjmp (jmpbuf, 1);
}
#endif
