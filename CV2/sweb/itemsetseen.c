
/*** ITEM_SET_SEEN.   CML function of same name.
/
/    item_set_seen (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $item_set_seen(cnum item userid resps_seen), 
/       return error code < 0, or else 0 = success.
/
/    How it works:
/       -1 means mark new
/       -2 means mark forgotten
/
/    Returns:
/
/    Error Conditions:
/
/    Home:  itemsetseen.c
/ */

/*** Copyright (C) 1995-2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 04/03/06 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "sqlcaucus.h"
#include "sql.h"

#define  SITE_CLEARED  -100
#define  QUERY_MAX     2000
#define  USER_MAX       256

#define TEMP(n)   chxtoken (temp, nullchix, n, arg)

extern int       cml_debug;
extern AsciiBuf  sqlbuf;

FUNCTION  item_set_seen (Chix result, Chix arg) {
   static Chix     temp = nullchix;
   int    error, item_id, ok, rc;
   int4   cnum, value;
   char   magic[USER_MAX], userid[USER_MAX];
   AsciiBuf ascii_buf();
   ENTRY ("item_add", "");

   /*** Initialization. */
   if (temp == nullchix) {
      temp     = chxalloc (L(40), THIN, "item_del temp");
   }

   /*** Parse all of the arguments. */
   error = 0;
   TEMP(1);   if (! chxnum (temp, &cnum))     cnum    = 0;
   TEMP(2);   if (! chxnum (temp, &item_id))  item_id = 0;
   TEMP(3);   ascofchx (userid, temp, 0, USER_MAX-1);
   TEMP(4);   if (! chxnum (temp, &value)) value = -3;

   /*** Validate arguments and access. */
   if      (cnum    ==  0)  error = ERR_NOCNUM;
   else if (item_id <   0)  error = ERR_NOITEM;
   else if (value   <= -3)  error = ERR_INVALID;

   if (error == 0) {
      if (value == -1) 
         sprintf (sqlbuf->str, 
            "DELETE FROM resps_seen WHERE userid='%s' AND items_id=%d",
            userid, item_id);
      else 
         sprintf (sqlbuf->str, 
            "REPLACE INTO resps_seen (userid, items_id, seen, forgot) "
            " VALUES ('%s', %d, %d, %d) ", userid, item_id, 
            (value >= 0 ? value : 0), (value == -2 ? 1 : 0));
      rc = sql_quick_limited (0, sqlbuf->str, magic, 100, 0);
      if (rc == 0)  error = ERR_FAILED;
   }

   sprintf (magic, "%d", error);
   chxcat  (result, CQ(magic));
   return  (1);
}
