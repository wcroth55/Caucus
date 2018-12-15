/*** SYSCHKPRV. Check to make sure that the Caucus executable has
/               sufficient priviliges to run.
/
/   ok = syschkprv (path, num)
/
/   Parameters:
/     int   ok;     // Return value, 0=yes
/     char *path;   // Full path of executable
/     int   num;    // Unused for now
/
/   Purpose:
/      See main/chkprv.c
/
/   How it works:
/      Unix: check setuid bit.
/      VMS:
/
/   Returns: 0:  success
/            1:  Unix setuid bit missing
/            9:  Generic Unix problem
/            11: VMS SYSPRV missing
/            12: VMS SYSLCK missing
/            20: Unidentified error
/
/   Errors:
/
/   Related functions:  main/chkprv()
/
/   Called by:
/
/   Operating System Dependencies:
/
/   Home: sys/syschkpr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  4/20/93 14:43 Create this. */
/*: JV  6/08/93 20:23 Changt path to char ptr. */
/*: CP  8/09/93 13:15 Put compiler error in VV code as flag. */
/*: CK 10/05/93 12:55 Add 1st draft of VV code. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "handicap.h"
#include "systype.h"
#include "chixuse.h"

FUNCTION  syschkprv (path, num)
   char *path;
   int   num;
{

/*---------------------NT-------------------------------------------*/
#if NUT40 | WNT40
   return (0);
#endif

/*---------------------Unix-----------------------------------------*/
#if UNIX
   int    found;
   struct stat buf;

   found = stat (path, &buf);

   if (found == -1)            return (9);
   if (buf.st_mode & S_ISUID)  return (0);
   else                        return (1);
#endif

}
