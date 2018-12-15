/*** SYSCOPY.   Fast copy of (any) file A to file B.
/
/    ok = syscopy (a, b);
/
/    Parameters:
/       int    ok;          (Success?)
/       char  *a;           (name of source file)
/       char  *b;           (name of destination file)
/
/    Purpose:
/       Copy file A to file B.
/
/    How it works:
/
/    Returns: 1 on success
/             0 can't open file A or B.
/
/    Error Conditions:
/ 
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies: heavy
/
/    Known bugs:      none
/
/    Home:  sys1/syscopy.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#if NUT40
#include <fcntl.h>
#endif

#if WNT40
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "rwmacro.h"

FUNCTION  syscopy (a, b)
   char  a[], b[];
{
   int   fda, fdb, len;
   char  buf[1024];

   ENTRY ("syscopy", "");

   sysbrkignore();

/*---------------------------------------NT-------------------------------*/
#if NUT40
   fda = open  (a, O_RDONLY | O_BINARY);
   fdb = creat (b, 0600     | O_BINARY);
#endif

#if WNT40
   if ( (fda = _open (a, O_RDONLY | O_BINARY)) >= 0)
         fdb = _open (b, O_WRONLY | O_BINARY | O_CREAT, _S_IREAD | _S_IWRITE);
#endif

/*---------------------------------------UNIX-----------------------------*/
#if UNIX
   if ( (fda = open  (a,    0)) >= 0)
         fdb = creat (b, 0600);
#endif

   sysbrkinit();
 
   if (fda < 0)                    RETURN (0);
   if (fdb < 0)  { _CLOSE (fda);   RETURN (0); }

   for (len=1024;   (len = _READ (fda, buf, len)) > 0;)  _WRITE (fdb, buf, len);
   _CLOSE (fda);
   _CLOSE (fdb);

   RETURN (1);
}
