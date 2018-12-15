
/*** HOSE_ACCEPT.   Accept an incoming connection through a hose.
/
/    fd = hose_accept (conf, timeout);
/ 
/    Parameters:
/       Hose   fd;                    (returned file descriptor for hose)
/       struct sweb_conf_t *conf;     (swebs configuration info)
/       int                 timeout;  (return with BADHOSE after this time)
/
/    Purpose:
/
/    How it works:
/
/    Returns: connected hose, or BADHOSE on time-out.
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  hoseacce.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/15/97 11:26 New function. */
/*: CR  6/04/97 17:21 Merged with old hose_prep Unix code. */

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
#include <setjmp.h>
static jmp_buf      jmpbuf;
#endif

#if NUT40
#include <fcntl.h>
#endif

#if WNT40
#include <windows.h>
#include <iostream.h>
#include <string.h>
#include <stddef.h>
#include <process.h>
#endif

FUNCTION  Hose hose_accept (struct sweb_conf_t *conf, int timeout)
{
   char   temp[256];

#if UNIX
   /*** The Unix version uses local sockets to connect subserver and client. */
   static  struct sockaddr_un subserv, client;
   static  int                sd = -1;
   int                        clientlen, cd;
   void                       timeouter();
   ENTRY ("hose_accept", "");

   if (sd < 0) {
      /*** The subserver must first create its own rendevous socket SD,
      /    based on its pid, so that the client knows where to find us. */
      sd = socket (AF_UNIX, SOCK_STREAM, 0);
      if (sd < 0) {
         perror ("subserver: error creating stream socket");
         exit   (3);
      }
      subserv.sun_family = AF_UNIX;
      sprintf (subserv.sun_path, "%s/SOCKET/sweb%06d", 
               conf->caucus_path, (int) getpid());
      unlink  (subserv.sun_path);
      if (bind (sd, (struct sockaddr *) &subserv, sizeof(subserv)) < 0) {
         perror ("subserver: error binding socket");
         exit   (4);
      }
   
      /*** Tell the kernel we're listening for a connection. */
      if (listen (sd, 1) < 0) {
         perror ("subserver: error listening");
         exit   (5);
      }
   }

   signal (SIGALRM, timeouter);
   alarm  (timeout);

   if (setjmp (jmpbuf) != 0)  RETURN (BADHOSE);

   clientlen = sizeof(client);
   cd = accept (sd, (struct sockaddr *) &client, &clientlen);
   alarm (0); 
  
   if (cd < 0) {
      perror ("subserver: error accepting connection");
      exit   (6);
   }

   RETURN (cd);
#endif


#if NUT40
   static int pd;
   char       pipename[256];
   int        pid, success;
   ENTRY ("hose_accept", "");

   /*** Just open a named pipe, with a name based on our pid, and
   /    be ready to read from it. */
   sprintf (pipename, "//./pipe/sweb%06d", getpid());
   pd = open (pipename, O_BINARY | O_RDWR | O_CREAT);
   if (pd < 0) {
      sprintf (temp, "error opening pipe, errno=%d", errno);
      logger  (1, LOG_FILE, temp);
      exit    (2);
   }

   5 = ;  /* this code doesn't handle time-out yet! */

   /*** Do a blocking read, waiting for a new swebsock client
   /    to write "our" pid to the named pipe open on 'hose'. */
   while (1) {
      read  (pd, &pid, sizeof(pid));
      success = (pid == getpid() ? pid+1 : 0);
      write (pd, &success, sizeof(success));
      if (success)  RETURN (pd);
   }
#endif


#if WNT40
   HANDLE     pipe;
   DWORD      xfer;
   int        pid, success;
   char       pipename[256];
   HANDLE     event;
   OVERLAPPED overlap;
   DWORD      wait_time;
   SECURITY_DESCRIPTOR sd;
   SECURITY_ATTRIBUTES sa;

   ENTRY ("hose_accept", "");

   /*** Just open a named pipe, with a name based on our pid, and
   /    be ready to read from it.  (Create it with a NULL DACL, so
   /    that anyone can access it.) */
   InitializeSecurityDescriptor (&sd, SECURITY_DESCRIPTOR_REVISION);
   SetSecurityDescriptorDacl (&sd, TRUE, NULL, FALSE);
   sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   sa.bInheritHandle = FALSE;
   sa.lpSecurityDescriptor = &sd;

   sprintf (pipename, "\\\\.\\pipe\\sweb%06d", getpid());
   pipe = CreateNamedPipe (pipename, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
             PIPE_TYPE_BYTE | PIPE_WAIT,
             1, 0, 0, 150, &sa);

   if (pipe == INVALID_HANDLE_VALUE) {
      sprintf (temp, "error opening pipe, errno=%d", errno);
      logger  (1, LOG_FILE, temp);
      exit    (2);
   }

   /*** Let client connect to us within the timeout period. */
   event = CreateEvent (0, FALSE, FALSE, 0);
   overlap.hEvent = event;

   ConnectNamedPipe (pipe, &overlap);
   wait_time = 1000 * timeout;
   if (WaitForSingleObject (event, wait_time) != WAIT_OBJECT_0)
      RETURN (BADHOSE);
   CloseHandle (event);


   /*** Do a blocking read, waiting for a new swebsock client
   /    to write "our" pid to the named pipe open on 'hose'. */
   while (1) {
      pid = 0;
      xfer = hose_read (pipe, &pid, sizeof(pid), 0);
      success = (pid == getpid() ? pid+1 : 0);
      xfer = hose_write (pipe, &success, sizeof(success));
      if (success)  RETURN (pipe);
   }
#endif

}

#if UNIX
FUNCTION  void timeouter (int value)
{
   longjmp (jmpbuf, 1);
}
#endif
