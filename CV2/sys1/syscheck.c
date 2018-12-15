
/*** SYSCHECK.  Check if a file exists.
/
/    SYSCHECK returns 1 if FILE exists, otherwise 0.
/    
/    Note that for client/server code, syscheck should only be used
/    to check on the existence of a file local to the client. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR  4/11/03 Use <errno.h> */
/*: CR  1/14/04 Add ENOTDIR to list of errno's that cause bail-out. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#if NUT40
#include <nutc/stat.h>
#endif

#if UNIX
#include <errno.h>
#endif


FUNCTION  syscheck (file)
   char file[];
{

#if NUT40
   struct _Nut_faststat  fast;
   int    ret;

   ret = _Nut_faststat (file, &fast, sizeof(fast));
   return (ret == 0);
#endif


#if WNT40
   return (_access (file, 0) == 0);
#endif


#if UNIX
   int  fd;

   ENTRY ("syscheck", "");

   if ( (fd = sysopen (file, 0, "syscheck")) >= 0)  close (fd);

   RETURN  (fd >= 0);
}


FUNCTION  sysopen (file, code, caller)
   char file[], caller[];
   int  code;
{
   char *bugtell();
   int  fd, i, open();
   char bug[100];

   ENTRY ("sysopen", "");

   for (i=0;   i<15;   ++i) {
      if ((fd = open (file, code)) >= 0)  RETURN (fd);
      if (errno == ENOENT || errno == EPERM  || errno == EACCES  ||
          errno == EXDEV  || errno == ENODEV || errno == ENOTDIR ||
          errno == ENFILE || errno == EMFILE || errno == EMLINK  ||
          errno == EDEADLK)     RETURN (-1);
/*    if (errno == ENOTDIR) break; */
      sleep (2);
      sysbrkclear();
      if (i > 2)  bugtell ("Waiting for an open file", errno, L(0), file);
   }
   sprintf (bug, "Sysopen failed: %s", caller);
   bugtell (bug, errno, L(0), file);
   sysexit (1);

   RETURN  (-1);
#endif

}
