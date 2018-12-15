
/*** SWEBSOCK.H.   Include file for SWEB socket use. */
 
/*** Copyright (C) 1997 Screen Porch LLC.  All rights reserved.  */

/*: CR  6/19/97 20:37 New file. */

#include "systype.h"

#ifndef  SWEBSOCK_H
#define  SWEBSOCK_H  1

#if UNIX
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#define  INV_SOCK      -1
#define  BADSOCK(fd)  (fd < 0)
#define  BADSTAT(bd)  (bd < 0)
#define  SOCKPTR      struct sockaddr *
#endif

#if NUT40
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#define  INV_SOCK      -1
#define  BADSOCK(fd)  (fd < 0)
#define  BADSTAT(bd)  (bd < 0)
#define  SOCKPTR      struct sockaddr *
#endif

#if WNT40
#include <windows.h>
#include <iostream.h>
#include <winsock.h>
#define  INV_SOCK      INVALID_SOCKET
#define  BADSOCK(fd)  (fd == INVALID_SOCKET)
#define  BADSTAT(bd)  (bd == SOCKET_ERROR)
#define  SOCKPTR      LPSOCKADDR
#endif

#endif
/*---------------------------------------------------------------------*/
