
/*** SYSTMPDIR.  Determine the directory for temporary files on this system.
/
/    systmpdir (dir, set);
/
/    Parameters:
/       char   *dir;       (directory name)
/       int     set;       (0=>get, 1=>set)
/
/    Purpose:
/       Get (or set) the full pathname for the directory for
/       temporary files on this system.
/
/    How it works:
/       systmpdir (str, 0)  puts the current temporary directory pathname
/       in STR.  (The default is "/tmp").
/
/       systmpdir (str, 1)  sets the current temporary directory to STR.
/       (This is ignored for Windows/NT.)
/
/    Known bugs:      none
/
/    Home:  sys/systmpdi.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/01/97 17:11 New function. */
/*: CR  6/11/98 15:05 Add 2nd arg. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "systype.h"

#if NUT40 | WNT40
#include <windows.h>
#include <iostream.h>
#endif

FUNCTION  systmpdir (char *dir, int set)
{

#if UNIX
#define  TEMPMAX  256
   static char tempdir[TEMPMAX] = {'/', 't', 'm', 'p', 0};

   if (set  &&  strlen (dir) < TEMPMAX)  strcpy (tempdir, dir);

   if (NOT set)                          strcpy (dir, tempdir);
#endif

#if WNT40
   char  *ptr, *getenv();

   ptr = getenv ("TMPDIR");
   if (ptr == NULL  ||  ptr[0] == '\0')   ptr = getenv ("TEMP");
   if (ptr == NULL  ||  ptr[0] == '\0')   ptr = getenv ("TMP");
   if (ptr == NULL  ||  ptr[0] == '\0')   ptr = "C:\\TEMP";
   strcpy (dir, ptr);

#endif

   return (1);
}
