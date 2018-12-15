
/*** SYS_TUSERS.   Return total number of registered users.
/
/    num = sys_tusers(confid);
/
/    Parameters:
/       int    num;         (total number of registered users)
/       Chix   confid;      (Caucus directory path)
/
/    Purpose:
/       Compute the number of registered Caucus users.
/   
/    How it works:
/       Count up the number of directories in USER001, USER002, etc.
/
/    Returns: number of registered users.
/
/    Error Conditions:
/ 
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  sys2/systuser.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/30/95 11:49 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "systype.h"

FUNCTION  sys_tusers (confid)
   Chix   confid;
{
   int    usernnn, total, start;
   int    error;
   char   fname[200], userdir[300];

   total = 0;

#if UNIX | NUT40 | WNT40

   for (usernnn=1;   (1);   ++usernnn) {
      sprintf (userdir, "%s/USER%03d", ascquick(confid), usernnn);

      for (start=1;   sysgfdir (userdir, fname, start, &error) == 1;   start=0)
         ++total;

      if (start == 1)  break;

   }

#endif

   return (total);
}
