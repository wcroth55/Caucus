
/*** RESP_SET.   CML function of same name.
/
/    resp_set (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $resp_set(), return error code < 0, or else
/       new response number.
/
/    How it works:
/       $resp_set (item_id rnum field value)
/
/       field: time, userid, text, prop, copy
/
/    Returns:
/
/    Error Conditions:
/  
/    Home:  respset.c
/ */

/*** Copyright (C) 1995-2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 04/03/06 New function. */
/*: CR 06/17/10 Add new version for field='text' if resps.versioned = 1. */

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

FUNCTION  resp_set (Chix result, Chix arg) {
   static Chix   temp = nullchix;
   static Chix   temp2;
   static AsciiBuf bigquery;
   int    error, rc, ok, i, item_id, len, rnum, value, versioned, highestVersion;
   int4   zp, prop, bits;
   char   nothing[100], magic[USER_MAX], field[USER_MAX];
   AsciiBuf ascii_buf();
   char *copyColumns;
   ENTRY ("resp_set", "");

   /*** Initialization. */
   if (temp == nullchix) {
      temp  = chxalloc     (L(40), THIN, "func_var temp");
      temp2 = chxalloc     (L(40), THIN, "func_var temp2");
      bigquery = ascii_buf (NULL, 10000);
   }

   /*** Parse all of the arguments. */
   error = 0;
   TEMP(1);   if (! chxnum (temp, &item_id)) item_id = 0;
   TEMP(2);   if (! chxnum (temp, &rnum))    rnum = -1;
   TEMP(3);   ascofchx (field, temp, 0L, USER_MAX-1);

   /*** Value: could be string or number. */
   chxtoken (nullchix, temp, 4, arg);
   chxclear (temp2);
   func_escsingle (temp2, temp);
   len = chxlen(temp2);
   ascii_buf (sqlbuf, len + 20);
   ascofchx  (sqlbuf->str, temp2, 0L, sqlbuf->max - 2);
   if (! chxnum (temp2, &value))    value = 0;

   /*** Validate arguments and access. */
   if      (item_id <  0)  error = RESP_ADD_NOITEM;
   else if (rnum    <  0)  error = RESP_SET_NORNUM;

   /*** Versioned responses create a new version for field=text. */
   if (streq (field, "text")) {
      sprintf (bigquery->str, 
         "SELECT versioned FROM resps WHERE items_id=%d AND rnum=%d AND version=0",
                 item_id, rnum);
      rc = sql_quick_limited (0, bigquery->str, magic, 100, 1);
      versioned = 0;
      if (rc == 1  &&  magic[0])  versioned = atoi(magic);
      versioned = (rc == 1  &&  magic[0] ? atoi(magic) : 0);
      if (versioned) {
         highestVersion = getHighestVersion (item_id, rnum);

         copyColumns = 
            "rnum, items_id, userid, text, time, prop, bits, copy_rnum, "
            "copy_item, copier, copy_time, copy_show, copy_cnum, "
            "deleted, author_shows_as, edittime, editid, versioned ";

         sprintf (bigquery->str, 
            "INSERT INTO resps (version, %s) "
            "   SELECT %d, %s FROM resps WHERE items_id=%d AND rnum=%d AND version=%d",
             copyColumns, highestVersion+1, copyColumns, item_id, rnum, highestVersion);
         rc = sql_quick_limited (0, bigquery->str, nothing, 100, 0);
         if (rc == 0)  error = RESP_SET_NOSET;
      }
   }

   if (streq (field, "time")      ||
       streq (field, "approved")  ||
       streq (field, "userid")    ||
       streq (field, "text")) {
      highestVersion = getHighestVersion (item_id, rnum);
      ascii_buf (bigquery, len + 200);
      sprintf (bigquery->str, 
         "UPDATE resps SET %s = '%s' WHERE rnum=%d AND items_id=%d  AND version=%d",
         field, sqlbuf->str, rnum, item_id, highestVersion);
      rc = sql_quick_limited (0, bigquery->str, nothing, 100, 0);
      if (rc == 0)  error = RESP_SET_NOSET;
   }

   else if (streq (field, "prop")    ||
            streq (field, "bits")) {
      sprintf (bigquery->str, 
         "UPDATE resps SET %s = %d WHERE rnum=%d AND items_id=%d AND version=0",
         field, value, rnum, item_id);
      rc = sql_quick_limited (0, bigquery->str, nothing, 100, 0);
      if (rc == 0)  error = RESP_SET_NOSET;
   }

   /*** And return the error code, or else the new response number. */
   sprintf (magic, "%d", (error == 0 ? 1 : error));
   chxcat  (result, CQ(magic));
}

FUNCTION int getHighestVersion (int itemId, int rnum) {
   char query[1000], magic[100];
   int  rc, result;

   sprintf (query,
      "SELECT MAX(version) FROM resps WHERE items_id=%d AND rnum=%d", itemId, rnum);
   rc = sql_quick_limited (0, query, magic, 100, 1);
   result = (rc == 1 ? atoi(magic) : 0);
   return result;
}
