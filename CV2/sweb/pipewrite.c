
/*** PIPEWRITE.   Write a buffer to a pipe in 'chunks'.
/
/    ok = pipewrite (hose, buf, len, chunk);
/   
/    Parameters:
/       int      ok;       (write successful)
/       Hose     hose;     (hose to write to)
/       char    *buf;      (data to be written)
/       int      len;      (length of data in BUF)
/       int      chunk;    (chunk size)
/
/    Purpose:
/       Write a buffer to a pipe, but do it in multiple writes
/       of size CHUNK.
/
/    How it works:
/
/    Returns: 1 on success, 0 on error (broken FD).
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  pipewrite.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/06/95 19:04 New function. */

#include <stdio.h>
#include "systype.h"
#include "chixuse.h"
#include "sweb.h"

#if UNIX | NUT40
#include <sys/types.h>
#include <sys/socket.h>
#endif

FUNCTION  pipewrite (
   Hose  hose, 
   char *buf, 
   int   len, 
   int   chunk)
{
   int    amount, pos;

   for (pos=0;  pos < len;   pos += chunk) {
      amount = (len - pos > chunk) ? chunk : len - pos;

#if NUT40 | WNT40
      if (hose_write (hose, buf+pos, amount)              < 0)   return (0); 
#endif

#if UNIX
      if (send  (hose, buf+pos, amount, (unsigned int) 0) < 0)   return (0); 
#endif
   }

   return (1);
}
