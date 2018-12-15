
/*** SET_LASTON.   Update user's "laston" time field.
/
/    set_laston();
/   
/    Parameters:
/
/    Purpose:
/
/    How it works:
/
/    Home:  sweb/setlaston.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 02/07/05 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "sqlcaucus.h"
#include "sql.h"

FUNCTION  set_laston() {
   static Chix   uid = nullchix;
   char          query[1000], nothing[10];
   ENTRY ("set_laston", "");

   if (uid == nullchix)  uid = chxalloc (L(40), THIN, "set_laston name");

   sysuserid (uid);
   if (chxlen(uid) > 0) {
      sprintf   (query, "UPDATE user_info SET laston = now() WHERE userid='%s'",
                         ascquick(uid));
      sql_quick_limited (0, query, nothing, 10, 0);
   }
   RETURN    (0);
}
