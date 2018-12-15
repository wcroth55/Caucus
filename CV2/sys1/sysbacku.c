
/*** SYSBACKUP.   Return the name of the backup file.
/
/    Given the name of a Caucus FILE, return the name of the matching
/    backup file in BACKUP. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:43 Source code master starting point */
/*: CR  8/03/88 16:37 VM changes only */
/*: CR 11/02/88 16:04 Remove VM code altogether. */
/*: CR 11/17/88 14:30 PN changes only. */
/*: CW  5/08/89 15:17 Add PX system code. */
/*: JV 10/04/89 17:35 Stripped out AOS non-server stuff. */
/*: CR  9/20/90 17:03 Add PS system type. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

FUNCTION  sysbackup (backup, file)
   char   backup[];
   char   file[];
{

/*---------------------------------------FORTUNE-------------------------*/
#if UNIX | NUT40 | WNT40
   ENTRY ("sysbackup", "");

   strconc (backup, file, "_b");
#endif

   RETURN (1);
}
