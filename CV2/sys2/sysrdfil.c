
/*** SYSRDFILE.   Read a line of bytes from a disk file N.
/
/    ok = sysrdfile (n, buf, maxbuf);
/
/    Parameters:
/       int   ok;      (success?)
/       int   n;       (unit number of open file)
/       char  buf[];   (char array to hold bytes)
/       int   maxbuf;  (max size of BUF)
/
/    Purpose:
/       Read a line of bytes from the disk file open on unit N,
/       and place them (without any interpretation) into BUF.
/
/    How it works:
/       Reads up to MAXBUF-1 bytes and places them in BUF,
/       followed by a 0 byte.  Any trailing newlines are removed.
/       
/    Returns: 1 on success
/             2 on success, but did not reach newline
/             0 on end-of-file (or error, like file N not open)
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: lots
/
/    Known bugs: VM manner of determining end of file on remote
/       files is pretty ugly!
/
/    Home:  sys/sysrdfil.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/29/92 15:24 New function, split off from old unit_read(). */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

extern struct unit_template units[];

FUNCTION  sysrdfile (n, buf, maxbuf)
   int    n;
   char   buf[];
   int    maxbuf;
{
   int    last, code;
   char  *bugtell();

   ENTRY ("sysrdfile", "");

   buf[0] = '\0';

   if (units[n].access != READ) {
      bugtell ("sysrdfile: ", n, L(0), "reading from unopened file!\n");
      RETURN  (0);
   }

/*--------------------------UNIX etc.---------------------------*/
#if UNIX | NUT40 | WNT40
 {
   if (fgets (buf, maxbuf-1, units[n].fp) == NULL) {
      buf[0] = '\0';
      RETURN (0);
   }

   last = strlen (buf);
   code = (last < maxbuf-10 ? 1 : 2);
   while (buf[last-1]=='\012'  ||  buf[last-1]=='\015') {
      buf[--last] = '\0';
      code = 1;
   }
   RETURN (code);
 }
#endif

}
