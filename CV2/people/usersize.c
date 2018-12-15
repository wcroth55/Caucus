/*** USERSIZE.  Return the size of the user, in KB.
/
/  size = usersize (userid)
/
/  Parameters:
/    int    size   (size in KB or 0 if failed)
/    Chix   user   (userid)
/
/  Purpose:
/
/  How it works:
/
/  Returns: 0 or the amount of disk space that the user takes up
/
/  Error Conditions:
/
/  Side Effects:
/
/  Related to:
/
/  Called by: 
/
/  Home: 
/     conf/usersize.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  3/26/93 15:35 Create this. */
/*: JV  9/08/93 09:53 Add ENTRY. */

#include <stdio.h>
#include "handicap.h"
#include "unitcode.h"
#include "chixuse.h"

FUNCTION  usersize (user)
   Chix user;
{
   char  dname[200], fname[200];
   int   usize=0, error, ok;
   int4  fsize;

   ENTRY ("usersize", "");

   unit_name (dname, XUDD, L(0), L(0), L(0), L(0), user);

   for (ok = sysgfdir (dname, fname, 1, &error); ok == 1; 
        ok = sysgfdir (dname, fname, 0, &error)) {
      if (NOT sysfsize (fname, &fsize, &error)) RETURN (0);
      usize += fsize;
   }

   if (ok == 2) {RETURN (usize); }
   else         RETURN (0);
}
