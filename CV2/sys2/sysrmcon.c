
/*** SYSRMCONF.  Remove (delete) a conference directory.
/
/    Delete the conference directory for conference number N,
/    and all the files under it. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "unitwipe.h"

/*ARGSUSED*/

FUNCTION  sysrmconf (n)
   int    n;
{

#if UNIX | NUT40 | WNT40
   char   name[80];

   ENTRY ("sysrmcon", "");

   unit_name (name, XCDD, L(n), L(0), L(0), L(0), nullchix);

#if HPXA9 | WNT40
   unit_wipe (LCNF);
#endif

   sysrmdir  (name);
   RETURN (1);
#endif

}
