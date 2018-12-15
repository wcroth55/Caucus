
/*** ITEM_SET.   CML function of same name.
/
/    item_set (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $item_set(), return error code < 0, or else 0 = success.
/       $item_set (cnum item property value)
/
/    How it works:
/       $item_set (cnum item hidden  1/0)
/       $item_set (cnum item retired 1/0)
/       $item_set (cnum item frozen  1/0)
/       $item_set (cnum item userid  new-owner-userid)
/
/    Returns:
/
/    Error Conditions:
/
/    Home:  itemset.c
/ */

/*** Copyright (C) 1995-2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 04/03/06 New function. */
/*: CR 09/13/07 Do not test for access level, must be done by calling CML code. */

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

FUNCTION  item_set (Chix result, Chix arg) {
   static Chix     temp = nullchix;
   static Chix     temp2;
   int    error, item_id, ok, rc, len;
   int4   cnum, value;
   char   magic[USER_MAX], property[USER_MAX], strtemp[USER_MAX];
   AsciiBuf ascii_buf();
   ENTRY ("item_add", "");

   /*** Initialization. */
   if (temp == nullchix) {
      temp     = chxalloc (L(40), THIN, "item_del temp");
      temp2    = chxalloc (L(40), THIN, "item_del temp2");
   }

   /*** Parse all of the arguments. */
   error = 0;
   TEMP(1);   if (! chxnum (temp, &cnum))     cnum    = 0;
   TEMP(2);   if (! chxnum (temp, &item_id))  item_id = 0;
   TEMP(3);   ascofchx (property, temp, 0, USER_MAX-1);
              strlower (property);

   /*** Value, 4th+ token, could be string *or* number. */
   chxtoken (nullchix, temp, 4, arg);
   chxclear (temp2);
   func_escsingle (temp2, temp);
   ascofchx (strtemp, temp2, 0L, USER_MAX-1);
   if (! chxnum (temp2, &value)) value = 0;


   /*** Validate arguments and access. */
   if      (cnum    == 0)  error = ERR_NOCNUM;
   else if (item_id <  0)  error = ERR_NOITEM;

   /*** Does user have access to set this item? */
   /*   MUST BE CONTROLLED BY CML INTERFACE, NOT AT API LEVEL
   if (error == 0) {
      ok  = (conf_access(cnum) >= privValue("instructor"));
      if (NOT ok)          error = ERR_NOWRITE;
   } */

   if (error == 0) {
      if (streq (property, "hidden")   ||
          streq (property, "retired")) {
         sprintf (sqlbuf->str, 
            "UPDATE conf_has_items SET %s=%d WHERE items_id=%d", 
            property, value, item_id);
      }

      else if (streq (property, "frozen")) {
         sprintf (sqlbuf->str, 
            "UPDATE items SET frozen=%d WHERE id=%d", value, item_id);
      }

      else if (streq (property, "userid") ||
               streq (property, "title")) {
         sprintf (sqlbuf->str, 
            "UPDATE items SET %s='%s' WHERE id=%d", property, strtemp, item_id);
      }
      else error = ERR_INVALID;
   }

   if (error == 0) {
      rc = sql_quick_limited (0, sqlbuf->str, magic, 100, 0);
      if (rc == 0)  error = ERR_FAILED;
   }

   sprintf (magic, "%d", error);
   chxcat  (result, CQ(magic));
   return  (1);
}
