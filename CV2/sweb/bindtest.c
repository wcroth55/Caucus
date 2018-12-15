/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** BINDTEST.C    Test 'bind' problems on different file systems.
/
/    Invoked as:
/       bindtest   directory
/
/    Arguments:
/       directory    place where we'll try to create a socket file
/                    and bind to it.
/
/    Purpose:
/       Some file systems (?) don't seem to want to allow a 'bind'
/       to a socket.  Attempt to find out more information about why.
/
/    How it works:
/
/    Exit status: 0 on success, 1 on failure.
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  bindtest.c
/ */

/*: CR  6/06/99 00:04 New program. */

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#define  SOCKPTR      struct sockaddr *


  main (int argc, char *argv[])
{
   int    rd, status;
   struct sockaddr_un server;
   char sfile[200];


   if (argc < 2) {
      fprintf (stderr, "Usage: bindtest directory\n");
      exit   (1);
   }

   sprintf (sfile, "%s/sweb", argv[1]);
   unlink  (sfile);

   /*** Create RD, a "rendevous" socket, where clients can meet us, and bind
   /    it to a specific file name (Unix). */
   rd = socket (AF_UNIX, SOCK_STREAM, 0);
   if (rd < 0) {
      printf ("Can't create stream socket, errno=%d\n", errno);
      exit   (3);
   }

   server.sun_family = AF_UNIX;
   sprintf (server.sun_path, "%s/sweb", argv[1]);

   status = bind (rd, (SOCKPTR) &server, sizeof(server));
   if (status < 0) {
      printf ("Can't bind socket, path='%s/sweb', errno=%d\n", argv[1], errno);
      exit   (4);
   }

   /*** Tell the kernel we're listening for connections on RD. */
   status = listen (rd, 5);
   if (status < 0) {
      printf ("Can't listen on socket, errno=%d\n", errno);
      exit   (5);
   }

   sleep (2);
   printf ("Seems to work OK!\n");

   close   (rd);

   exit (0);
}
