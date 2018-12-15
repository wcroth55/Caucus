/*** SYSHOMEDIR.  Construct the pathname of the "home" directory for a user.
/
/   ok = syshomedir (dir);
/
/   Parameters:
/     int   ok;     (Return value)
/     Chix  dir;    (Place to put result)
/
/   Purpose:
/      Some files, e.g. temporary files, might be best placed in the user's
/      'home' directory.
/
/   How it works:
/
/   Returns: 1 on success, 0 otherwise
/
/   Errors:
/
/   Related functions:  systmpfile
/
/   Called by:
/
/   Operating System Dependencies:
/
/   Home: sys/syshomed.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  4/14/93 14:43 Create this. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "handicap.h"
#include "systype.h"
#include "chixuse.h"

FUNCTION  syshomedir (dir)
   Chix  dir;
{
#if UNIX | NUT40 | WNT40
   char *getenv();
   char *homedir;

   if ( (homedir = getenv ("HOME")) == NULL)  homedir = ".";
   chxofascii (dir, homedir);

#endif

   return (1);
}
