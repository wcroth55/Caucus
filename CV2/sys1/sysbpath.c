/*** SYSBPATH.  Build a full pathname from a directory name and a file name.
/
/    ok = sysbpath (result, dname, fname);
/
/    Parameters:
/       int    ok;      (success?)
/       char  *result;  (sysbpath writes into here)
/       char  *dname;   (full pathname of the directory)
/       char  *fname;   (unqualified file name)
/
/    Purpose:
/       Sysgfdir hands you a file name.  Sometimes application code needs the
/       full path.
/       SYSBPATH does that.
/
/    How it works:
/       SYSBPATH assumes that the caller knows what it's doing, and does
/       not check its arguments.
/
/    Returns: 1 on success, 0 on failure (not implemented)
/
/    Error Conditions:  (see syserror.h.)
/  
/    Side effects:
/
/    Related functions: sysgfdir()
/
/    Called by:  application.
/
/    Operating system dependencies: path name syntax
/
/    Known bugs:      none
/
/    Home:  sys/sysbpath.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JZ 10/22/93 12:00 Create this. */

#include <stdio.h>
#include "handicap.h"
#include "systype.h"
#include "syserror.h"

FUNCTION  sysbpath (result, dname, fname)
   char  *result, *dname, *fname;
{

/*------------------------------Most UNIXes------------------------------*/
#if UNIX | NUT40 | WNT40

   ENTRY ("sysbpath", "");

   strcpy (result, dname);
   strcat (result, "/");
   strcat (result, fname);
   RETURN (1);
#endif

   RETURN (0);
}
