
/*** FIX_NAME.  Fix user's full name to include "(userid)". */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/12/90 18:12 New function. */
/*: DE  4/10/92 11:57 Chixified */
/*: CR 12/10/92 15:31 chxindex 2nd arg is int4. */

#include <stdio.h>
#include "caucus.h"

extern Chix  ss_luid, ss_ruid;

extern union null_t null;

FUNCTION  fix_name  (reg)
   Userreg    reg;
{
   Chix   str, oldname;
   int    success;

   ENTRY ("fix_name", "");

   str      = chxalloc (L(80), THIN, "fixname str");
   oldname  = chxalloc (L(80), THIN, "fixname oldname");
   success  = 0;

   /*** If the user's name already contains "(userid)", nothing
   /    needs to be done. */
   chxformat (str, CQ("%s%s"), L(0), L(0), ss_luid, reg->id);
   chxformat (str, CQ("%s"),   L(0), L(0), ss_ruid, null.chx);
   if (chxindex (reg->name, L(0), str) >= 0)  SUCCEED;

   /*** The new name is just the old name with the "(userid)". */
   chxcpy  (oldname, reg->name);
   chxcat  (reg->name, CQ(" "));
   chxcat  (reg->name, str);

   /*** Change the name in the system-wide nameparts database, plus
   /    in every conference the user is a member of. */
   chg_full (oldname, reg->lastname, reg->name, reg->lastname);

   SUCCEED;

 done:

   chxfree ( str );
   chxfree ( oldname );

   RETURN ( success );
}
