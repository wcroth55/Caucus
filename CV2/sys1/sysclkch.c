/*** SYSCLKCHK.   Clock Check.  Has the system clock been set back?
/
/    SYSCLKCHK(0) checks the current time (NOW) against the time stored in
/    the binary XSCK file (THEN).  If THEN is later than NOW + 7200, the
          system
/    clock was set back, and we return 0.  Otherwise, everything's OK and
/    we return 1.  (The 7200--two hours--is to allow for time zone changes
/    and the vagaries of individual PCs connected to a network.)
/
/    SYSCLKCHK(1) resets the value in the XSCK file to NOW and returns 1. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#if WNT40
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "rwmacro.h"

#if NUT40
#include <fcntl.h>
#endif

FUNCTION  sysclkchk (reset)
   int    reset;
{
#if UNIX | NUT40 | WNT40
   char   name[200];
   int    fd, good;
   int4   then, now;
   int4   systime();

   ENTRY ("sysclkchk", "");

   /*** Lock the XSCK file and prepare to check NOW against THEN. */
   if (NOT unit_lock (XSCK, WRITE, L(0), L(0), L(0), L(0), nullchix))  RETURN (0);
   unit_name (name, XSCK, L(0), L(0), L(0), L(0), nullchix);
   now  = systime();
   then = now;
   good = 1;

   sysbrkignore();

#if UNIX
   fd = (reset ? creat (name, 0700) : 
                 open  (name, 2));
#endif

#if NUT40
   fd = (reset ? creat (name, 0700 | O_BINARY) : 
                 open  (name, O_RDWR | O_BINARY));
#endif

#if WNT40
   fd = (reset ? _open (name, O_RDWR | O_BINARY | O_CREAT, _S_IREAD|_S_IWRITE) :
                 _open (name, O_RDWR | O_BINARY));
#endif

   sysbrkinit();

   /*** Read THEN; if ok, rewrite XSCK with NOW. */
   if (fd >= 0) {
      if (NOT reset) {
         _READ  (fd, (char *) &then, 4);
         _LSEEK (fd, L(0), 0);
      }
      if (now >= then)  _WRITE (fd, (char *) &now, 4);
      _CLOSE (fd);
   }

   good = (fd >= 0)  &&  (now >= then - 7200);

   unit_unlk (XSCK);

   RETURN (good);
#endif

}
