
/*** SHAKE_SUBSERVER.  "Shake hands" with subserver, and confirm
/    that we can request a file from it. 
/
/    rc = shake_subserver (sockpath, subpid, subcode, userid, 
/                          caucus_ver, auth_method, hose);
/   
/    Parameters:
/       int    rc;         (return code)
/       char  *sockpath,   (path to socket directory)
/       int    subpid,     (pid of desired subserver)
/       int    subcode,    (security code for desired subserver)
/       char  *userid,     (browser user's userid)
/       char  *caucus_ver  (cookie verification code)
/       char   auth_method (authentication method 1=http, 2=cookie)
/       Hose  *hose;       (returned hose connection to subserver)
/
/    Purpose:
/       "Shake hands" with the desired subserver, i.e. try to connect
/       to it, and see if it thinks we are it's designated user.
/
/    How it works:
/       Opens a socket (Unix) or named pipe (NT) to the subserver 
/       pid # SUBPID, tell it who we are, and see if it likes us. 
/       Sets HOSE to opened hose, or BADHOSE on failure.
/
/    Returns: 1 on success
/             0 if request rejected
/            -1 on socket communication error (can be retried after delay)
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/shakesub.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/27/96 16:13 New function.  Based on old swebcli.c */
/*: CR  4/15/97 15:55 Integrate with NT. */
/*: CR  1/26/99 23:12 Changed args to get return code. */
/*: CR 10/07/01 15:00 Raise userid length. */
/*: CR  4/02/02 12:40 Cookie login support. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "sweb.h"

#if UNIX | NUT40
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#endif

#include <time.h>
#include <signal.h>
#include <errno.h>


FUNCTION  int shake_subserver (
   char  *sockpath,
   int    subpid,
   int    subcode,
   char  *userid,
   char  *caucus_ver,
   char   auth_method,
   Hose  *hose)
{
   Hose   sockfd, hose_open();
   int    success;
   char   temp[256], remote[256];

   /*** Open a connection to the "hose" the subserver is listening on. */
   *hose = BADHOSE;
   sockfd = hose_open (sockpath, subpid, userid);
   if (sockfd == BADHOSE)  return (-1);

   /*** OK, the subserver's there.  Tell it who we are, and see if it
   /    will serve us. */
   sprintf (temp, "%07d", subcode);
   hose_write   (sockfd, temp,    7);
   hose_write   (sockfd, userid, MAX_USERID);
   hose_write   (sockfd, &auth_method, 1);
   hose_write   (sockfd, caucus_ver, 10);
   env_var (remote, "REMOTE_ADDR");
   hose_write   (sockfd, remote, 25);

   /*** Nope, it didn't like us.  Could be the user is trying to spoof
   /    swebd; or maybe it's a "stale" URL with a subpid that has been
   /    re-used by someone else. */
   hose_read (sockfd, temp, 3, 0);
   if (strcmp (temp, "OK") != 0)  {
      hose_close (sockfd);
      if (strcmp (temp, "NO") == 0)  return (0);
      logger (1, LOG_FILE, "socket comm error");
      return (-1);
   }

   *hose = sockfd;
   return (1);
}
