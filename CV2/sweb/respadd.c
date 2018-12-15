
/*** RESP_ADD.   CML function of same name.
/
/    resp_add (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $resp_add(), return error code < 0, or else
/       new response number.
/
/    How it works:
/       $resp_add (cnum item_id userid_author property bits 
/                  reserved text)
/
/    Returns:
/
/    Error Conditions:
/  
/    Home:  respadd.c
/ */

/*** Copyright (C) 1995-2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 04/03/06 New function. */
/*: CR 05/13/08 Use sql_last_insert() instead of SELECT LAST_INSERT_ID. */
/*: CR 09/16/09 Calculate maxRnum for new response 'by hand'. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "sqlcaucus.h"
#include "sql.h"
#include "caucus5.h"

#define  SITE_CLEARED      -100
#define  QUERY_MAX         2000
#define  USER_MAX           256
#define  MAX_ADD_RESP_TRIES  10
#define  SELECT_FAILED       -2

#define TEMP(n)   chxtoken (temp, nullchix, n, arg)

extern int       cml_debug;
extern AsciiBuf  sqlbuf;

FUNCTION  resp_add (Chix result, Chix arg) {
   static Chix   temp = nullchix;
   static Chix   temp2;
   static AsciiBuf bigquery;
   int    error, rc, ok, i, lastresp, 
          item_id, frozen, len, maxRnum, tries, newRnum;
   int4   cnum, zp, prop, bits;
   int    rnum;
   char   user[USER_MAX], nothing[100], magic[USER_MAX], field[USER_MAX],
          title[USER_MAX], rnumBuf[100];
   AsciiBuf ascii_buf();
   ENTRY ("resp_add", "");

   /*** Initialization. */
   if (temp == nullchix) {
      temp  = chxalloc     (L(40), THIN, "func_var temp");
      temp2 = chxalloc     (L(40), THIN, "func_var temp2");
      bigquery = ascii_buf (NULL, 10000);
   }

   /*** Parse all of the arguments. */
   error = 0;
   TEMP(1);   if (! chxnum (temp, &cnum))    cnum    = 0;
   TEMP(2);   if (! chxnum (temp, &item_id)) item_id = 0;
   TEMP(3);   ascofchx (user, temp, 0L, USER_MAX-1);
   TEMP(4);   if (! chxnum (temp, &prop))    prop = 0;
   TEMP(5);   if (! chxnum (temp, &bits))    bits = 0;

   /*** Item Text */
   chxtoken (nullchix, temp, 7, arg);
   chxclear (temp2);
   func_escsingle (temp2, temp);
   len = chxlen(temp2);
   ascii_buf (sqlbuf, len + 20);
   ascofchx  (sqlbuf->str, temp2, 0L, sqlbuf->max - 2);

   /*** Validate arguments and access. */
   if      (cnum    == 0)  error = RESP_ADD_NOCNUM;
   else if (item_id <  0)  error = RESP_ADD_NOITEM;

   /*** Is this item readonly? */
   if (error == 0) {
      ok  = (conf_access(cnum) >= privValue("include"));
      if (NOT ok)          error = RESP_ADD_NOWRITE;
   }

   /*** Is this item frozen? */
   if (error == 0) {
      sprintf (bigquery->str, "SELECT frozen FROM items WHERE id=%d",
               item_id);
      rc = sql_quick_limited (0, bigquery->str, magic, 100, 1);
      frozen = 0;
      if (rc == 1  &&  magic[0])  frozen = atoi(magic);
      if (frozen)          error = RESP_ADD_NOWRITE;
   }

   /*** Insert the row for the new response, and get its key. */
   if (error == 0) {

      for (tries=0;   tries<MAX_ADD_RESP_TRIES;   ++tries) {
         sprintf (bigquery->str, "SELECT MAX(rnum) FROM resps WHERE items_id=%d AND version=0",
                  item_id);
         rc = sql_quick_limited (0, bigquery->str, magic, 100, 1);
         maxRnum = SELECT_FAILED;
         if (rc == 1  &&  magic[0])     maxRnum = atoi(magic);
         if (maxRnum == SELECT_FAILED)  {
            rc = 0;
            break;
         }
   
         newRnum = maxRnum + 1;
         ascii_buf (bigquery, sqlbuf->max + 200);
         sprintf (bigquery->str, 
            "INSERT INTO resps (rnum, items_id, userid, text, time, prop, bits) "
            "VALUES (%d, %d, '%s', '%s', now(), %d, %d)",
                           newRnum, item_id, user, sqlbuf->str, prop, bits);
         rc = sql_quick_limited (0, bigquery->str, nothing, 100, 0);
         if (rc != 0)  break;
         syssleep(2);
      }
      if (rc == 0)  error = RESP_ADD_FAILED;
      lastresp = maxRnum + 1;
   }

   /*** If adding response #0, delete fake response -1 row. */
   if (error == 0  &&  lastresp == 0) {
      sprintf (bigquery->str, 
         "DELETE FROM resps WHERE items_id = %d AND rnum < 0", item_id);
      rc = sql_quick_limited (0, bigquery->str, nothing, 100, 0);
   }

   /*** Update items.lastresp to ensure it has highest non-deleted response number.  But it's tricky,
   /    because scanning over lots of deleted responses is slow. */
   if (error == 0) {
      rnumBuf[0] = '\0';
      sprintf (bigquery->str, 
         "SELECT MAX(rnum) FROM resps WHERE items_id = %d AND version=0 ", item_id);
      rc = sql_quick_limited (0, bigquery->str, rnumBuf, 100, 1);

      if (rc == 0  ||  ! strnum(rnumBuf, &rnum)) error = RESP_ADD_NOLAST;
   }
   if (error == 0) {
      int rSpan = 50;
      while (1) {

         sprintf (bigquery->str, 
            "SELECT MAX(rnum) FROM resps WHERE items_id = %d AND rnum > %d AND deleted=0 AND version=0 ", 
            item_id, rnum - rSpan);
         rc = sql_quick_limited (0, bigquery->str, rnumBuf, 100, 1);
         
         if (rc == 0)                  break;
         if (strnum (rnumBuf, &rnum))  break;
         if (rnum - rSpan < 0)         break;

         rSpan = 2 * rSpan;
      }
      if (rc == 0)  error = RESP_ADD_NOLAST;
   }
   if (error == 0) {
      sprintf (bigquery->str, 
         "UPDATE items SET lastresp = %d"
         " WHERE id = %d ", rnum, item_id);
      rc = sql_quick_limited (0, bigquery->str, nothing, 100, 0);
      if (rc == 0)  error = RESP_ADD_NOLAST;
   }

   /*** And return the error code, or else the new response number. */
   sprintf (magic, "%d", (error == 0 ? lastresp : error));
   chxcat  (result, CQ(magic));
}
