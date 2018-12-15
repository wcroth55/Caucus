
/*** HOSE_READ.   Read from a Hose (file or pipe).
/
/    num = hose_read (fh, buf, size, timeout);
/ 
/    Parameters:
/       int    num;         (number of bytes read, or 0, or -1)
/       Hose   fh;          (file or pipe handle or descriptor)
/       char  *buf;         (place read bytes here)
/       int    size;        (how many bytes to read)
/       int    timeout;     (return after this many seconds regardless)
/
/    Purpose:
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  sweb/hoseread.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/03/97 17:16 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

#if WNT40
#include <windows.h>
#include <iostream.h>
#endif

FUNCTION  int hose_read (Hose fh, char *buf, int size, int timeout)
{

#if UNIX | NUT40
   /*** Ignores timeout, which is fine. */
   return (read (fh, buf, size));
#endif


#if WNT40
   HANDLE     event;
   OVERLAPPED overlap;
   int        success, xfer;

#if USE_OVERLAP
   event = CreateEvent (0, TRUE, FALSE, 0);
   overlap.hEvent     = event;
   overlap.Offset     = 0;
   overlap.OffsetHigh = 0;
   timeout = (timeout==0 ? INFINITE : 1000 * timeout);

   success = 1;
   if (NOT ReadFile (fh, buf, size, &xfer, &overlap) ) {
      if (GetLastError() == ERROR_IO_PENDING) {
         success = (WaitForSingleObject (event, timeout) == WAIT_OBJECT_0);
         if (success)  GetOverlappedResult (fh, &overlap, &xfer, FALSE);
      }
      else success = 0;
   }
   CloseHandle (event);

#else
   success = ReadFile (fh, buf, size, &xfer, (LPOVERLAPPED) NULL);
#endif

   return (success ? xfer : -1);
#endif

}
