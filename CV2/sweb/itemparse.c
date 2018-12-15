
/*** ITEM_PARSE.   CML function of same name.
/
/    item_parse (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $item_parse(), return error code < 0, or else 0 = success.
/
/    How it works:
/       $item_parse (cnum itemlabel[:responseNumber])
/       $item_parse (cnum text)
/
/    Returns:
/       First case evaluates to a triplet {cnum itemid rnum}.
/       Second case evaluates to a triplet list {cnum itemid 0}*.
/       Errors evaluate to a negative number.
/       Lack of match evaluates to empty string.
/
/    Error Conditions:
/
/    Home:  itemparse.c
/ */

/*** Copyright (C) 2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 03/01/07 New function. */
/*: CR 01/03/11 Parse '*', as in item 5.* */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "sqlcaucus.h"
#include "sql.h"
#include "caucus5.h"

#define  USER_MAX       256

#define TEMP(n)   chxtoken (temp, nullchix, n, arg)

#define FAIL(n)   { error = n;   goto failed; }

extern AsciiBuf  sqlbuf;

FUNCTION  item_parse (Chix result, Chix arg) {
   static Chix     temp = nullchix;
   static Chix     temp2;
   static AsciiBuf query;
   AsciiBuf ascii_buf();
   int    error, rc, i, item_id, success, colon, rnum;
   int4   cnum, c;
   int4  *get_item_id();
   int4  *itemids;
   char   magic[USER_MAX], str[USER_MAX];
   ENTRY ("item_id", "");

   /*** Initialization. */
   if (temp == nullchix) {
      query = ascii_buf(NULL, 1000);
      temp  = chxalloc (L(40), THIN, "item_parse temp");
      temp2 = chxalloc (L(40), THIN, "item_parse temp2");
   }

   error = 0;
   TEMP(1);   if (! chxnum (temp, &cnum)) FAIL(ITEM_ID_NOCNUM);

   /*** Try to parse a numeric item label. */
   TEMP(2);
   if (chxvalue(temp, 0) == '*'  ||  chxnum (temp, &c))  {
      rnum  = 0;
      colon = chxindex (temp, 0, CQ(":"));
      if (colon > 0) {
         ascofchx (str, temp, colon+1, 100);
         rnum = max (atoi(str), 0);
      }

      itemids  = get_item_id(cnum, temp);
      magic[0] = '\0';
      for (i=0;   itemids[i] > 0;   ++i) {
         sprintf (magic, "%d %d %d ", cnum, itemids[i], rnum);
         chxcat  (result, CQ(magic));
      }
      fflush (stdout);
      sysfree (itemids);
      if (NULLSTR(magic)) FAIL(ITEM_ID_NOITEM);
   }

   /*** Otherwise, must be part of an item title. */
   else {
      chxtoken (nullchix, temp, 2, arg);
      chxclear       (temp2);
      func_escsingle (temp2, temp);

      query = ascii_buf (query, chxlen(temp2) + 200);
      sprintf (query->str, 
         "SELECT %d, i.id, 0 FROM items i, conf_has_items h "
         " WHERE i.id=h.items_id AND h.cnum=%d AND i.title LIKE '%%%s%%'",
         cnum, cnum, ascquick(temp2));

      append_query_results (query->str, result, sqlbuf);
   }

   return 1;

failed:
   sprintf (magic, "%d", (error < 0 ? error : item_id));
   chxcat  (result, CQ(magic));
   return 0;
}
