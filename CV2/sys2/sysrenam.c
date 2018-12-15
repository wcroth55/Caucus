
/*** SYSRENAME.  Renames file A to file B.
/
/    Returns 1 if successful, and 0 otherwise.
/
/    For server/client programs, SYSRENAME must be (is) used only
/    to rename local files. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern int debug;

FUNCTION  sysrename (a, b)
   char   a[], b[];
{
   ENTRY  ("sysrename", "");

/*---------------------------------------NT------------------------------*/
#if NUT40
   if (rename (a, b) < 0)  {
      syscopy (a, b);
      unlink  (a);
   }
#endif

#if WNT40
   if (rename (a, b) < 0)  {
      syscopy (a, b);
      _unlink  (a);
   }
#endif


/*-------------------------------FORTUNE & MASSCOMP & A3 | LA-----------------*/
#if UNIX
   if (rename (a, b) < 0) {
      syscopy (a, b);
      unlink (a);
   }
#endif
 
   RETURN (1);
}
