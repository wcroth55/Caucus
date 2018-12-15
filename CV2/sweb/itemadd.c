
/*** ITEM_ADD.   CML function of same name.
/
/    item_add (result, arg, conf);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/       struct sweb_conf_t *conf,
/
/    Purpose:
/       Evaluate $item_add(), return error code < 0, or else
/       new item number (e.g. 5, 3.2.17, etc.)
/
/    How it works:
/       $item_add (cnum parent_item_id parent_response
/                  userid-author title1 [title2...] )
/
/    Returns:
/
/    Error Conditions:
/  
/    Home:  itemadd.c
/ */

/*** Copyright (C) 1995-2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 03/31/06 New function. */
/*: CR 02/10/11 Re-use highest deleted item # or breakout #. */

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
static AsciiBuf magic = NULL;

AsciiBuf ascii_buf();

FUNCTION  item_add (Chix result, Chix arg, struct sweb_conf_t *conf)
{
   static Chix     temp = nullchix;
   static Chix     temp2;
   int    error, rc, ok, parent_item, parent_resp, add, i, j, item[7],
          item_id;
   int4   cnum, zp;
   char   user[USER_MAX], nothing[100], field[USER_MAX],
          title[USER_MAX];
   ENTRY ("item_add", "");

   /*** Initialization. */
   if (temp == nullchix) {
      temp  = chxalloc     (L(40), THIN, "func_var temp");
      temp2 = chxalloc     (L(40), THIN, "func_var temp2");
      ascii_buf (sqlbuf, 1000);
      magic = ascii_buf (NULL, 100);
   }

   /*** Parse all of the arguments. */
   error = 0;
   TEMP(1);   if (! chxnum (temp, &cnum))          cnum = 0;
   TEMP(2);   if (! chxnum (temp, &parent_item))   parent_item = 0;
   TEMP(3);   if (! chxnum (temp, &parent_resp))   parent_resp = 0;
   TEMP(4);   ascofchx (user, temp, 0L, USER_MAX-1);

   chxtoken (nullchix, temp, 5, arg);
   chxclear (temp2);
   func_escsingle (temp2, temp);
   ascofchx (title, temp2, 0L, USER_MAX-1);


   /*** Validate arguments and access. */
   if      (cnum == 0)        error = ITEM_ADD_NOCNUM;
   else if (parent_item < 0)  error = ITEM_ADD_IPARENT;
   else if (parent_resp < 0)  error = ITEM_ADD_RPARENT;

   /*** Can this user add an item? */
   if (error == 0) {
      ok  = (conf_access(cnum) >= privValue("include"));
      if (NOT ok)  error = ITEM_ADD_NOPERM;
   }

   /*** Create the actual "ghost" item, i.e. empty and not in the conf yet. */
   if (error == 0) {
      sprintf (sqlbuf->str,
         "INSERT INTO items (id, userid, title) "
         " VALUES (NULL, '%s', '%s')", user, title);
      rc = sql_quick_limited (0, sqlbuf->str, nothing, 100, 0);
      if (rc == 0)  error = ITEM_ADD_INSERT;
   }

   /*** Determine the (full, 6 part!) new item number. */
   if (error == 0) {
      item_id = last_insert_key();

      /*** Simple case, no parent.  Just get the next major (item0) number. */
      if (parent_item == 0) {
         sprintf (sqlbuf->str, 
            "SELECT MAX(item0) FROM conf_has_items WHERE cnum=%d AND deleted=0 ", cnum);
         rc = sql_quick (0, sqlbuf->str, magic, 1);
         if (rc == 0  ||  (item[0] = atoi(magic->str)) <= 0)  item[0] = 0;
         ++item[0];
         item[1] = item[2] = item[3] = item[4] = item[5] = 0;
      }

      /*** Hard case, handed a parent.  Get the parent's full item sextuplet,
      /    and use it to make the new item's sextuplet. */
      else {
         sprintf (sqlbuf->str, 
            "SELECT item0, item1, item2, item3, item4, item5 "
            "  FROM conf_has_items WHERE cnum=%d AND items_id=%d AND deleted=0",
            cnum, parent_item);
         rc = sql_exec (0, sqlbuf->str);
         if (rc == SQL_SUCCESS  &&  (rc = sql_fetch(0)) != SQL_NO_DATA) {
            for (i=0;   i<6;   ++i) {
               sql_getchardata (0, magic);   item[i] = atoi(magic->str);
            }
            sql_fetch(0);  /* implicitly close cursor */
         }
         else error = ITEM_ADD_GETITEM;

         /*** Find the last non-zero element in the item sextuplet. */
         if (error == 0) {
            for (i=0;   item[i] > 0  &&  i<6;   ++i)  ;
            if  (i >= 6)  error = ITEM_ADD_TOODEEP;
         }
   
         /*** Find the max number for the matching element for all
         /    children of the parent.  The new element number is the
         /    max + 1. */
         if (error == 0) {
            sprintf (sqlbuf->str, 
               "SELECT MAX(item%d) FROM conf_has_items "
               " WHERE cnum=%d AND items_parent=%d AND deleted=0 ", i, cnum, parent_item);
            rc = sql_quick (0, sqlbuf->str, magic, 1);
            if (rc == 0)  item[i] = 1;
            else          item[i] = atoi(magic->str) + 1;
            for (j=i+1;   j<6;   ++j)  item[j] = 0;
         }
      }
   }

   /*** Create the temporary "-1" row in resps, so that the first,
   /    auto-incremented, rnum will start at 0. */
   if (error == 0) {
      sprintf (sqlbuf->str, 
         "INSERT INTO resps (rnum, items_id) VALUES (-1, %d)", item_id);
      rc = sql_quick_limited (0, sqlbuf->str, nothing, 100, 0);
      if (rc == 0)  error = ITEM_ADD_AUTOINC;
   }

   /*** Finally, create the entry in conf_has_items that says this
   /    item belongs to this conference. */
   if (error == 0) {
      sprintf (sqlbuf->str, 
         "INSERT INTO conf_has_items (cnum, items_id, item0, item1, item2, "
         "              item3, item4, item5, items_parent, resps_parent) "
         " VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
         cnum, item_id, item[0], item[1], item[2], item[3], item[4],
         item[5], parent_item, parent_resp);
      rc = sql_quick_limited (0, sqlbuf->str, nothing, 100, 0);
      if (rc == 0)  error = ITEM_ADD_CONFHAS;
   }

   /*** And return the error code, or else the new item sextuplet. */
   sprintf (magic->str, "%d", (error < 0 ? error : item_id));
   chxcat  (result, CQ(magic->str));
}


FUNCTION int last_insert_key() {
   int   rc;
   magic = ascii_buf (magic, 100);

   strcpy (sqlbuf->str, "SELECT LAST_INSERT_ID()");
   rc = sql_quick (0, sqlbuf->str, magic, 1);
   if (rc == 0)       return (0);
   return (atoi(magic->str));
}

