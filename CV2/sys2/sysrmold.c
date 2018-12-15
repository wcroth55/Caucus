/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSRMDIR.  Completely remove directory PATH and all it's files. */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern Chix confid;

FUNCTION  sysrmdir (path)
   char   path[];
{

#if NUT40 | WNT40 | UNIX
   char  fname[256], fullname[256];
   int   start, error;

   ENTRY ("sysrmdir", "");

   for (start=1;   sysgfdir (path, fname, start, &error) == 1;  start=0) {
      sprintf (fullname, "%s/%s", path, fname);
#if NUT40 | UNIX
      unlink  (fullname);
#endif
#if WNT40
      _unlink (fullname);
#endif
   }

   rmdir (path);
#endif

   RETURN (1);
}
