/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSKILL.   Kill a user.
/
/    SYSKILL removes USER's personal USER and TEXT directories, and
/    all the contents of those directories.  */

/*: AA  7/29/88 17:59 Source code master starting point */
/*: CR  8/30/88 14:53 Change "#if SX" to "#if UNIX"! */
/*: JV  9/05/88 13:18 Added AOS code. */
/*: CR 11/16/88 13:21 Add SRV code. */
/*: CR 11/22/88 20:55 Add PN code. */
/*: CR  3/21/89 15:46 Add use of servername in #if SRV code. */
/*: CW  7/20/89 15:43 Add PX code. */
/*: CR  8/29/89 16:00 Add use of sysudnum() to get USER dir. number. */
/*: CR 10/24/89 13:23 Add 3rd diagnostic argument to sysudnum(). */
/*: CR 12/19/89 12:48 PN code only: close user lockfile before deleting user! */
/*: CR  3/27/90 13:47 Rearrange systype.h/units.h order. */
/*: DE  6/08/92 17:20 Chixified */
/*: CL 12/10/92 17:07 Long ascofchx args. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

extern Chix confid;

FUNCTION  syskill (user_chx)
   Chix   user_chx;
{
   char   user[80];
   char   path[80];

   ascofchx(user, user_chx, L(0), L(80));


/*-------------------------Unixes----------------------------------*/
#if UNIX | NUT40 | WNT40
   {
      int   ud;

      if ( (ud = sysudnum (user_chx, -1, 0)) <= 0)  return (0);
      sprintf  (path, "%s/USER%03d/%s", ascquick(confid), ud, user);
      sysrmdir (path);
      sprintf  (path, "%s/TEXT%03d/%s", ascquick(confid), ud, user);
      sysrmdir (path);
 }
#endif


   return (1);
}


