
/*** HOSE_WRITE.   Write to a hose.
/
/    hose_write (fh, buf, size);
/ 
/    Parameters:
/       Hose   fh;          (file or pipe handle or descriptor)
/       char  *buf;         (bytes to be written)
/       int    size;        (how many bytes to read)
/
/    Purpose:
/
/    How it works:
/
/    Returns:  n>0 on success, 0 otherwise
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  sweb/hosewrite.c
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

FUNCTION  int hose_write (Hose fh, char *buf, int size)
{

#if UNIX | NUT40
   write  (fh, buf, size);
   return (1);
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

   success = 1;
   if (NOT WriteFile (fh, buf, size, &xfer, &overlap) ) {
      if (GetLastError() == ERROR_IO_PENDING) {
         success = (WaitForSingleObject (event, INFINITE) == WAIT_OBJECT_0);
         if (success)  GetOverlappedResult (fh, &overlap, &xfer, FALSE);
      }
      else success = 0;
   }
   CloseHandle (event);

#else
   success = WriteFile (fh, buf, size, &xfer, (LPOVERLAPPED) NULL);
#endif

   return (success ? xfer : 0);
#endif
}
