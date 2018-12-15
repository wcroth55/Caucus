
/*** FIX_FULLPATH   Fix a relative filename to make it absolute.
/
/    fix_fullpath (name);
/   
/    Parameters:
/       char  *name;        (relative filename to be fixed)
/
/    Purpose:
/       Turn a (possibly) relative filename into an absolute
/       full pathname.
/
/    How it works:
/
/    Known bugs:
/
/    Home:  sweb/fixfull.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/13/98 17:39 New function. */


#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

#if UNIX | NUT40
#include <unistd.h>
#endif

#if WNT40
#include <direct.h>
#endif

#define  SLASH(x)  (x=='/'  ||  x=='\\')
#define  MAXVAL    512

FUNCTION  void fix_fullpath (char *name)
{
   int    slash, skip;
   char   cwd[MAXVAL];

   /*** Names beginning with a slash, or X:slash are full pathnames. */
   if (SLASH(name[0]))                    return;
   if (SLASH(name[2])  &&  name[1]==':')  return;

   /*** Otherwise, the name is relative to the current directory. 
   /    Get it, if possible. */
   if (getcwd (cwd, MAXVAL) == NULL)      return;

   /*** Skip over a possible "./" at the beginning of Name. */
   skip = 0;
   if (name[0]=='.'  &&  SLASH(name[1]))  skip = 2;

   /*** Assemble the full pathname. */
   strcat (cwd, "/");
   strcat (cwd, name+skip);
   strcpy (name, cwd);

   return;
}
