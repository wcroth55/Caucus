
/*** NEW_SUBSERVER.  Request a new subserver from the master swebd.
/
/    new_subserver (sockpath, subpid, subsecure, userid);
/   
/    Parameters:
/       char  *sockpath,    (path to socket directory)
/       int   *subpid,      (return pid of new subserver here)
/       int   *subsecure    (return security code of new subserver here)
/       char  *userid
/
/    Purpose:
/       Connect to the master swebd server, and request a new subserver
/       for the current browser user.
/
/    How it works:
/
/    Returns: 
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/newsub.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/27/96 16:13 New function.  Based on old swebcli.c */
/*: CR  4/15/97 12:53 Integrate with NT. */
/*: CR  3/05/02 12:55 Add userid arg. */

#include <stdio.h>
#include "systype.h"
#include "chixuse.h"
#include "api.h"

#include "sweb.h"
#include "swebsock.h"


FUNCTION  new_subserver (
   char  *sockpath,
   int   *subpid,
   int   *subsecure,
   char  *userid     )
{
   char   remote[100], pid[20], secure[20], temp[256];
   int    sockfd, status;
#if UNIX
   struct sockaddr_un server;
#endif
#if NUT40
   struct sockaddr_in server;
   struct hostent *host_info;
#endif
#if WNT40
   WSADATA         data;
   SOCKADDR_IN     server;
   unsigned int    destination;
#endif
   ENTRY ("new_subserver", "");

   /*** Create a socket, and connect it to the "file" that SWEBD 
   /    is listening on. */
#if UNIX
   sockfd = socket (AF_UNIX, SOCK_STREAM, 0);
#endif
#if NUT40
   sockfd = socket (AF_INET, SOCK_STREAM, 0);
#endif
#if WNT40
   WSAStartup (MAKEWORD(1,1), &data);
   sockfd = socket (AF_INET, SOCK_STREAM, 0);
#endif

   if (BADSOCK(sockfd)) {
      logger (1, LOG_FILE, "Swebsock cannot create stream socket");
      exit   (1);
   }

#if UNIX
   server.sun_family = AF_UNIX;
   sprintf (server.sun_path, "%s/sweb", sockpath);
#endif

#if NUT40
   host_info = gethostbyname ("localhost");
   if (host_info == NULL) {
      logger (1, LOG_FILE, "Swebsock cannot get host info.");
      exit   (1);
   }
   server.sin_family = host_info->h_addrtype;
   memcpy ((char *)&server.sin_addr.s_addr, 
                    host_info->h_addr, host_info->h_length);
   server.sin_port = htons(SERVER_PORT);
#endif

#if WNT40
   destination = inet_addr("127.0.0.1");
   memcpy (&server.sin_addr, &destination, sizeof(destination));
   server.sin_port   = htons(SERVER_PORT);
   server.sin_family = AF_INET;
#endif


   status = connect (sockfd, (SOCKPTR) &server, sizeof(server));
   if (BADSTAT(status)) {
      logger (1, LOG_FILE, "Swebsock cannot connect to swebd.");
      exit   (1);
   }

#if UNIX | NUT40
   write   (sockfd, userid, MAX_USERID);
   read    (sockfd, pid,    7);     pid   [7] = '\0';
   read    (sockfd, secure, 8);     secure[7] = '\0';
   close   (sockfd);
#endif

#if WNT40
   write(sockfd, userid, MAX_USERID);
   recv (sockfd, pid,    7, 0);     pid   [7] = '\0';
   recv (sockfd, secure, 8, 0);     secure[7] = '\0';
   closesocket (sockfd);
#endif

   sscanf  (pid,    "%d", subpid);
   sscanf  (secure, "%d", subsecure);

   sprintf (temp, "swebsock: newsub(): subpid=%d", *subpid);
   logger  (0, LOG_FILE, temp);

   RETURN (1);
}
