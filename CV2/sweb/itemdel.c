
/*** ITEM_DEL.   CML function of same name.
/
/    item_del (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $item_del(), return error code < 0, or else 0 = success.
/
/    How it works:
/       $item_del (cnum item_id)
/
/    Returns:
/
/    Error Conditions:
/
/    Notes: worry about attachments someday!
/  
/    Home:  itemdel.c
/ */

/*** Copyright (C) 1995-2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 04/03/06 New function. */
/*: CR 04/16/07 Fix query for finding subitems. */
/*: CR 09/13/07 Access level test must be done by calling CML code. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "sqlcaucus.h"
#include "sql.h"
#include "caucus5.h"

#define  SITE_CLEARED  -100
#define  QUERY_MAX     2000
#define  USER_MAX       256

#define TEMP(n)   chxtoken (temp, nullchix, n, arg)

extern int       cml_debug;
extern AsciiBuf  sqlbuf;

FUNCTION  item_del (Chix result, Chix arg) {
   static Chix     temp = nullchix;
   static Chix     itemlist, word;
   static AsciiBuf bigquery;
   int    error, rc, ok, i, item_id, delete;
   int4   cnum, pos;
   char   user[USER_MAX], nothing[100], magic[USER_MAX], item[USER_MAX];
   AsciiBuf ascii_buf();
   ENTRY ("item_add", "");

   /*** Initialization. */
   if (temp == nullchix) {
      temp     = chxalloc (L(40), THIN, "item_del temp");
      word     = chxalloc (L(40), THIN, "item_del word");
      itemlist = chxalloc (L(40), THIN, "item_del itemlist");
      bigquery = ascii_buf (NULL, 1000);
   }

   /*** Parse all of the arguments. */
   error = 0;
   TEMP(1);   if (! chxnum (temp, &cnum))    cnum = 0;
   TEMP(2);   if (! chxnum (temp, &item_id)) item_id = 0;

   /*** Validate arguments and access. */
   if      (cnum    == 0)  error = ITEM_DEL_NOCNUM;
   else if (item_id <  0)  error = ITEM_DEL_NOITEM;

   /*** Does user have access to delete this item? */
   /* MUST BE DONE BY CALLING CML CODE.
   if (error == 0) {
      ok  = (conf_access(cnum) >= privValue("organizer"));
      if (NOT ok)          error = ITEM_DEL_NOWRITE;
   } */

   /*** Done here if there were errors.  Otherwise, we forge on
   /    no matter what! */
   if (error < 0) {
      sprintf (magic, "%d", error);
      chxcat  (result, CQ(magic));
      return  (1);
   }

   /*** Find list of all subitems in this conference. */
   sprintf (bigquery->str, 
      "SELECT a.items_id FROM conf_has_items a, conf_has_items b "
      " WHERE a.cnum=%d AND b.cnum=a.cnum "
      "   AND b.items_id=%d "
      "   AND  a.item0=b.item0  "
      "   AND (a.item1=b.item1 OR b.item1=0) "
      "   AND (a.item2=b.item2 OR b.item2=0) "
      "   AND (a.item3=b.item3 OR b.item3=0) "
      "   AND (a.item4=b.item4 OR b.item4=0) "
      "   AND (a.item5=b.item5 OR b.item5=0) ", cnum, item_id);
   append_query_results (bigquery->str, itemlist, sqlbuf);

   /*** For each item/subitem, see if it is a singleton, or if
   /    other conferences have a link to this item. */
   for (pos=0;   chxnextword (word, itemlist, &pos) >= 0;  ) {
      ascofchx (item, word, 0, USER_MAX-1);
      sprintf (bigquery->str, 
         "SELECT count(*) FROM conf_has_items WHERE items_id=%s", item);
      rc = sql_quick_limited (0, bigquery->str, magic, 100, 1);

      /*** If it's a singleton, we can delete it entirely.  Otherwise,
      /    just remove it from conf_has_items. */
      delete = (rc == 1  &&  magic[0]  &&  atoi(magic) == 1);

      sprintf (bigquery->str, 
         "DELETE FROM conf_has_items WHERE items_id=%s AND cnum=%d", 
          item, cnum);
      rc = sql_quick_limited (0, bigquery->str, magic, 100, 0);

      /*** Delete it entirely, including responses, seen records, etc. */
      if (delete) {
         sprintf (bigquery->str, "DELETE FROM resps WHERE items_id=%s", item);
         rc = sql_quick_limited (0, bigquery->str, nothing, 100, 0);

         sprintf (bigquery->str, "DELETE FROM items WHERE id=%s", item);
         rc = sql_quick_limited (0, bigquery->str, nothing, 100, 0);

         sprintf (bigquery->str, "DELETE FROM resps_seen WHERE items_id=%s", word);
         rc = sql_quick_limited (0, bigquery->str, nothing, 100, 0);
      }
   }


   /*** And return the error code, or else the new response number. */
   chxcatval  (result, THIN, L('0'));
   return (1);
}
