
/*** BUF_WRITE.   Write to a hose, but with internal buffering.
/    BUF_FLUSH.   Flush internal buffering.
/    BUF_CLOSE.   Close a hose,   but flush internal buffering.
/
/    ok = buf_write (hose, buf, len);
/    buf_flush (hose);
/    buf_close (hose);
/   
/    Parameters:
/       int      ok;       (successful write?)
/       Hose     hose;       (hose to write or close)
/       char    *buf;      (data to be written)
/       int      len;      (length of data in BUF)
/
/    Purpose:
/
/    How it works:
/
/    Returns: 1 on success, 0 on error (such as broken hose).
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/bufwrite.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/24/97 22:47 New function, based on old mywrite. */
/*: CR  6/12/97 14:56 Rename, use hoses. */
/*: CR  4/11/03 Use <errno.h> */

#include <stdio.h>
#include <errno.h>
#include "chixuse.h"
#include "sweb.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

static  struct sockmsg_t sockmsg = {0, 0};
extern  int client_pid;
extern  FILE  *sublog;

FUNCTION  buf_write (
   Hose   hose, 
   char  *buf,
   int    len)
{
   int    pos, space, data, move, sent, ackgot;
   char   temp;
   char   temper[200];
   char  *prefix;
   ENTRY ("buf_write", "");

   if (len == 0)                                           RETURN (1);

   for (pos=0;   pos < len;   pos+=move) {
      space = SOCKMSGMAX - sockmsg.len;
      data  = len - pos;
      move = (data > space ? space : data);
      memcpy (sockmsg.buf + sockmsg.len, buf+pos, move);
      sockmsg.len += move;
      if (sockmsg.len == SOCKMSGMAX) {

#if UNIX
/*       errno = 0;   RA2 */
         sent   = send (hose, (char *) &sockmsg, sizeof(sockmsg), 0);

         #if DIAGNOSE
            prefix = " |||";
            if (sent <= 0)  prefix = " <<<";
            sprintf (temper, "NEW8: %s send bytes=%d to pid=%d, sent=%d, errno=%d", 
                prefix, sizeof(sockmsg), client_pid, sent, errno);
            logger (1, LOG_FILE, temper);
         #endif
#endif

#if NUT40 | WNT40
         sent   = hose_write (hose, &sockmsg, sizeof(sockmsg));
#endif

         /*** Simulate slow page. */
/*       logger (1, LOG_FILE, "(sockmsg) ");  */
/*       sleep (10);  */

#if LNX12
/*       ackgot = recv (hose, &temp, 1, MSG_WAITALL);   RA2 */
         ackgot = recv (hose, &temp, 1, 0);

         #if DIAGNOSE
            prefix = " |||";
            if (ackgot <= 0)  prefix = " <<<";
            sprintf (temper, "NEW9: %s recv 1 from pid=%d, ackgot=%d, errno=%d", prefix, client_pid, ackgot, errno);
            logger (1, LOG_FILE, temper);
         #endif

         if (ackgot <= 0)  sleep(1);
#endif
/*       fprintf (stderr, "sent=%d, errno=%d, ackgot=%d\n", 
                           sent, errno, ackgot);   */
         sockmsg.len = 0;
      }
   }

   RETURN (1);
}


FUNCTION  void buf_flush (Hose hose)
{
   int    ackgot, sent;
   char   temp;
   char   diagnose[100];

#if UNIX
   sent   = send (hose, (char *) &sockmsg, sizeof(sockmsg), 0);
#endif

#if NUT40 | WNT40
   sent   = hose_write (hose, &sockmsg, sizeof(sockmsg));
#endif

#if LNX12
   ackgot = recv (hose, &temp, 1, 0);
#endif

   sockmsg.len = 0;
}


FUNCTION  void buf_close (Hose hose)
{
   int    ackgot, sent;
   char   temp;

   sockmsg.eof = 1;

#if UNIX
   sent   = send  (hose, (char *) &sockmsg, sizeof(sockmsg), 0);
   ackgot = recv  (hose, &temp, 1, 0);
   close (hose);
#endif

#if NUT40 | WNT40
   sent   = hose_write (hose, &sockmsg, sizeof(sockmsg));
   ackgot = hose_read  (hose, &temp, 1, 0);
   hose_close (hose);
#endif

/* fprintf (stderr, "EOF: sent=%d, errno=%d, ackgot=%d\n", sent, errno, ackgot);  */

   sockmsg.len = 0;
   sockmsg.eof = 0;
   return;
}
