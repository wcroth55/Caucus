
/*** UNITMKSEQ.  Create a sequential file.
/
/    UNITMKSEQ creates and opens the sequential file associated with
/    unit N.  If USER is 1, UNITMKSEQ tries to create the file as
/    though the user were writing it.  Otherwise, it creates the file
/    with Caucus' privileges. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:54 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include "caucus.h"
#include <stdio.h>
#include "systype.h"
#include "units.h"

#if SUN41
#include <sys/stat.h>
#endif

extern struct unit_template units[];
extern int                  debug;

FUNCTION  unitmkseq (n, user)
   int    n, user;
{

/*-------------------------UNIX-------------------------------*/
#if UNIX | NUT40 | WNT40
   int    oldmask;

   ENTRY ("unitmkseq", "");

   if (user)  sysprv(0);
   if (NOT sysaccess (units[n].nameis))  units[n].fp = NULL;
   else {
/*    if (n==XIRE)  oldmask = umask (0);  */

#if UNIX | NUT40
      units[n].fp = fopen (units[n].nameis, "w");
#endif
#if WNT40
      nt_mkfile (units[n].nameis, 0700);
      units[n].fp = fopen (units[n].nameis, "wt");
#endif

      units[n].fd = -1;
/*    if (n==XIRE)  umask (oldmask);      */
   }
   if (user)  sysprv(1);

   RETURN (units[n].fp != NULL);
#endif

}
