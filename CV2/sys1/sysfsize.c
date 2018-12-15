
/*** SYSFSIZE.   Return the size of a file, in bytes.
/
/    ok = sysfsize (fname, bytes, error);
/
/    Parameters:
/       int    ok;          (success?)
/       char  *fname;       (full pathname of the file)
/       int4  *bytes;       (returned size of file)
/       int   *error;       (error code if failure)
/
/    Purpose:
/       Determine the size of a file.
/
/    How it works:
/
/    Returns: 2 (success, fname is a directory)
/             1 (success, fname is a file)
/             0 (failure, sets value of ERROR as below)
/
/    Error Conditions:  (see syserror.h.)
/       SE_NOTIMP     Function not implemented for this system.
/       SE_BADFIL     File FNAME does not exist.
/       SE_NOINFO     Cannot get information for unknown reason.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  application.
/
/    Operating system dependencies: considerable!
/
/    Known bugs:      none
/
/    Home:  sys/sysfsize.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/08/91 12:45 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "handicap.h"
#include "systype.h"
#include "entryex.h"
#include "syserror.h"
#include "int4.h"

#include <sys/types.h>
#include <sys/stat.h>

FUNCTION  sysfsize (fname, bytes, error)
   char  *fname;
   int4  *bytes;
   int   *error;
{
/*--------------------------System V Unix--------------------------------*/
   struct stat  statbuf;

   ENTRY  ("sysfsize", "");

   if (stat (fname, &statbuf))      { *error = SE_BADFIL;    RETURN (0); }
   if (statbuf.st_size < 0)         { *error = SE_NOINFO;    RETURN (0); }
   *error = 0;
   *bytes = statbuf.st_size;

/* printf ("sysfsize: size=%d\n", *bytes); */

#if UNIX
   RETURN (    S_ISDIR (statbuf.st_mode) ? 2 : 1);
#endif

#if WNT40
   RETURN ( (_S_IFDIR & statbuf.st_mode) ? 2 : 1);
#endif

}
