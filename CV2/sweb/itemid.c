
/*** ITEM_ID.   CML function of same name.
/
/    item_id (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $item_id(), return error code < 0, or else 0 = success.
/
/    How it works:
/       $item_id (cnum item(sextuplet))
/
/    Returns:
/
/    Error Conditions:
/
/    Home:  itemid.c
/ */

/*** Copyright (C) 2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 06/07/06 New function. */
/*: CR 01/03/11 Parse '*', as in item 5.* */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "sqlcaucus.h"
#include "sql.h"
#include "caucus5.h"

#define  USER_MAX       256

#define TEMP(n)   chxtoken (temp, nullchix, n, arg)

extern AsciiBuf  sqlbuf;

FUNCTION  item_id (Chix result, Chix arg) {
   static Chix     temp = nullchix;
   int    error, rc, i, item_id;
   int4  *itemids;
   int4   cnum;
   char   magic[USER_MAX];
   int4  *get_item_id();
   ENTRY ("item_id", "");

   /*** Initialization. */
   if (temp == nullchix) {
      temp     = chxalloc (L(40), THIN, "item_del temp");
   }

   /*** Parse all of the arguments. */
   error = 0;
   TEMP(1);
   if (! chxnum (temp, &cnum)) cnum = 0;
   TEMP(2);
   itemids = get_item_id(cnum, temp);
   item_id = itemids[0];
   sysfree (itemids);

   /*** Validate arguments and access. */
   if      (cnum    == 0)  error = ITEM_ID_NOCNUM;
   else if (item_id <= 0)  error = ITEM_ID_NOITEM;

   sprintf (magic, "%d", (error < 0 ? error : item_id));
   chxcat  (result, CQ(magic));
}

/*** Given the conf num and the item sextuplet, return the item_id. */
FUNCTION int *get_item_id (int cnum, Chix temp) {
   Chix itemids, word;
   char itemid[100];
   int  item[7] = {0, 0, 0, 0, 0, 0, 0};
   int  label, i, rc;
   int  itemidCount = 0;
   int4 pos, prv;
   int  resultIndex;
   char magic[100];
   char sql[1000];
   char andItem[7][40];

   for (i=0;   i<7;   ++i)  sprintf (andItem[i], "AND item%d=0 ", i);

   for (prv=pos=0, i=0;   i<6;   ++i) {
      if (chxvalue(temp, pos) == '*') {
         sprintf (andItem[i], "AND item%d>0 ", i);
         ++pos;
      }
      else {
         label = chxint4 (temp, &pos);
         if (pos == prv)  break;
         sprintf (andItem[i], "AND item%d=%d ", i, label);
      }
      if (chxvalue(temp, pos) != '.')  break;
      prv = ++pos;
   }
   if (pos == 0) {
      int *results = (int4 *) sysmem(sizeof(int4), "results");
      results[0] = -1;
      return (results);
   }

   itemids = chxalloc (200, THIN, "itemids");
   word    = chxalloc ( 32, THIN, "word");
   sprintf (sql,
      "SELECT items_id FROM conf_has_items WHERE cnum=%d %s %s %s %s %s %s AND deleted=0",
      cnum, andItem[0], andItem[1], andItem[2], andItem[3], andItem[4], andItem[5]);

   rc = sql_exec (0, sql);
   while (sql_fetch(0) != SQL_NO_DATA) {
      sql_getchardata(0, sqlbuf);
      chxcat    (itemids, CQ(sqlbuf->str));
      chxcatval (itemids, THIN, ' ');
      ++itemidCount;
   }

   int *results = (int4 *) sysmem(sizeof(int4) * (itemidCount+1), "results");
   for (pos=0, resultIndex=0;   chxnextword (word, itemids, &pos) >= 0;  ++resultIndex) {
      results[resultIndex] = atoi(ascquick(word));
   }
   results[resultIndex] = 0;

   chxfree (word);
   chxfree (itemids);
   return results;
}
