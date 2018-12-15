
/*** ITEM_LABEL.   CML function of same name.
/
/    item_label (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $item_label(), return error code < 0, or else 0 = success.
/
/    How it works:
/       $item_label (cnum item_id)
/
/    Returns:
/
/    Error Conditions:
/
/    Home:  itemlabel.c
/ */

/*** Copyright (C) 2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 06/07/06 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "sqlcaucus.h"
#include "sql.h"
#include "caucus5.h"

#define  USER_MAX       256

#define TEMP(n)   chxtoken (temp, nullchix, n, arg)

extern AsciiBuf  sqlbuf;

FUNCTION  item_label (Chix result, Chix arg) {
   static Chix     temp = nullchix;
   static AsciiBuf magic;
   int    error, rc, i, item_id;
   int4   cnum;
   char   word[USER_MAX];
   char  *strtoken();
   AsciiBuf ascii_buf();
   ENTRY ("item_id", "");

   /*** Initialization. */
   if (temp == nullchix) {
      temp     = chxalloc (L(40), THIN, "item_del temp");
      magic = ascii_buf (NULL, USER_MAX);
   }

   /*** Parse all of the arguments. */
   error = 0;
   TEMP(1);   if (! chxnum (temp, &cnum))    cnum    = 0;
   TEMP(2);   if (! chxnum (temp, &item_id)) item_id = 0;

   /*** Validate arguments and access. */
   if      (cnum    == 0)  error = ITEM_ID_NOCNUM;
   else if (item_id <  0)  error = ITEM_ID_NOITEM;

   sprintf (sqlbuf->str, 
      "SELECT item0, item1, item2, item3, item4, item5 "
      "  FROM conf_has_items WHERE cnum=%d AND items_id=%d AND deleted=0",
      cnum, item_id);
   rc = sql_exec (0, sqlbuf->str);
   if (rc == SQL_SUCCESS  &&  (rc = sql_fetch(0)) != SQL_NO_DATA) {
      for (i=0;   i<6;   ++i) {
         sql_getchardata (0, magic);
         if (! magic->str[0]  ||  magic->str[0]=='0')  break;
         if (i > 0)  chxcatval (result, THIN, L('.'));
         chxcat (result, CQ(magic->str));
      }
      sql_fetch(0);  /* implicitly close cursor */
   }
   return (1);
}
