
/*** SYSISDIR.  Is DIR a directory?
/
/    ok = sysisdir (dir);
/
/    Parameters:
/       int    ok;             (success?)
/       char  *dir;            (directory name)
/
/    Purpose:
/       Determine if DIR exists and is a directory.
/
/    How it works:
/       
/    Returns: 1 if DIR is a directory, 0 otherwise
/
/    Operating system dependencies: heavy
/
/    Known bugs:
/
/    Home:  sys1/sysisdir.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/16/97 17:00 New function. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

#if WNT40
#include <sys/types.h>
#endif
#include <sys/stat.h>

FUNCTION  sysisdir (char *dir)
{

#if UNIX | NUT40
   struct stat  statbuf;

   if (stat (dir, &statbuf) != 0)              return (0);
   if ( (statbuf.st_mode & 070000) != 040000)  return (0);
   return (1);
#endif

#if WNT40
   struct _stat statbuf;

   if (_stat (dir, &statbuf) != 0)             return (0);
   if (NOT (statbuf.st_mode & _S_IFDIR))       return (0);
   return (1);
#endif

}
