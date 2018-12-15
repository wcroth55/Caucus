/*** SYSDIRCHK.  Directory Check.  Makes sure that the user has 
/    the USERS and TEXTS directories needed for the participation
/    files and text-editing files, respectively. 
/
/    Returns 1 on success, 0 if failure (which is probably
/    catastrophic.) */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern Chix confid;

FUNCTION  sysdirchk(Chix thisuser)
{
   char  thisuser_str[40], confid_str[200];
   char  dir[80];
   int   udnum;

   ENTRY ("sysdirchk", "");

   /*** Handle the USERS directory, which must remain accesible only
   /    to the caucus id. */
   udnum = sysudnum (thisuser, 1, 0);
   if (udnum <= 0)  RETURN (0);

   ascofchx  (thisuser_str, thisuser, L(0),  L(40));
   ascofchx  (confid_str,   confid,   L(0), L(200));

   /*** Handle the TEXTS directory, which everybody can conceivably 
   /    access. */
   sprintf (dir, "%s/TEXT%03d/%s", confid_str, udnum, thisuser_str);
   if (NOT syscheck (dir))  sysmkdir (dir, 0777, NULL);

   RETURN (1);
}
