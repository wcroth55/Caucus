
/*** FUNC_VAR.   variable functions.
/
/    func_var (result, what, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       char               *what;    (function name)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Append value of $user_var(), $conf_var(), $user_info()
/       function WHAT with arguments ARG to RESULT.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:  should we refresh things automatically after a
/                 certain period of time, in case someone else
/                 put new information in?
/
/    Home:  funcvar.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/06/95 12:56 New function. */
/*: CR  1/18/02 16:35 touch_cv(), test cnum for > 0 */
/*: CR  3/01/02 14:00 make sure sitevars are loaded before list_site_var() */
/*: CR  3/31/04 touch_cv() succeeds even if no var file (otherwise can't
/               create new ones, e.g. in set_item_var!) */
/*: CR  1/28/05 Add $user_info(). */
/*: CR 10/26/05 Make $set_user_data(), $set_site_data() escape single 
/               quotes in *name* ! */
/*: CR 01/17/06 Make $userids_byid() filters by active >= 10 */
/*: CR 11/10/10 $add_user_info() should not add an index for mediumtext fields. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"
#include "sqlcaucus.h"
#include "sql.h"

#define  SITE_CLEARED  -100
#define  QUERY_MAX     2000
#define  USER_MAX       256

extern int       cml_debug;
extern AsciiBuf  sqlbuf;

FUNCTION  func_var (Chix result, char *what, Chix arg)
{
   static Chix   name = nullchix;
   static Chix   temp, uid, temp2;
   static Vartab cvar, uvar, ivar, svar;
   static AsciiBuf bigquery;
   int    ok, slot, code, num, empty, update, rc, len, total, actual, found;
   int4   cnum;
   char   query[QUERY_MAX], query2[QUERY_MAX], 
          user[USER_MAX], nothing[100], type[USER_MAX], magic[USER_MAX],
          descr[USER_MAX], ascname[USER_MAX],
          ascCnum[USER_MAX], ascInum[USER_MAX], ascVname[USER_MAX];
   AsciiBuf ascii_buf();
   ENTRY ("func_var", "");

   /*** Initialization. */
   if (name == nullchix) {
      name  = chxalloc     (L(40), THIN, "func_var name");
      temp  = chxalloc     (L(40), THIN, "func_var temp");
      temp2 = chxalloc     (L(40), THIN, "func_var temp2");
      uid   = chxalloc     (L(40), THIN, "func_var uid");
      cvar  = a_mak_vartab ("func_var cvar");
      uvar  = a_mak_vartab ("func_var uvar");
      ivar  = a_mak_vartab ("func_var ivar");
      svar  = a_mak_vartab ("func_var svar");
      svar->id1 = SITE_CLEARED;
      bigquery = ascii_buf (NULL, 10000);
   }

   /*** $CONF_VAR(cnum vname). */
   if (streq (what, "conf_var")) {
      if (touch_cv (cvar, arg, 0)) {
         chxtoken (name, nullchix, 2, arg);
         slot = vartab_find (cvar, name);
         chxcat (result, cvar->value[slot]);
      }
   }

   /*** $CLEAR_CONF_VAR() */
   else if (streq (what, "clear_conf_var")) {
      cvar->id1 = -1;
   }

   /*** $SET_CONF_VAR(cnum vname value). */
   else if (streq (what, "set_conf_var")) {
      if (touch_cv (cvar, arg, 0)) {
         chxtoken (name, nullchix, 2, arg);
         chxtoken (nullchix, temp, 3, arg);
         slot = vartab_find (cvar, name);
         if (NOT chxeq (temp, cvar->value[slot])) {
            chxcpy (cvar->value[slot], temp);
            ok = (a_chg_vartab (cvar, 0, A_WAIT) == A_OK);
         }
      }
   }

   /*** $LIST_CONF_VAR(cnum vroot) */
   else if (streq (what, "list_conf_var")) {
      if (touch_cv (cvar, arg, 0)) {
         empty = (chxtoken (name, nullchix, 2, arg) < 0);
         for (num=0;   num<cvar->used;   ++num) {
            if (empty  ||  chxindex (cvar->name[num], 0, name) == 0) {
               chxcat (result, cvar->name[num]);
               chxcatval (result, THIN, L(' '));
            }
         }
      }
   }

   else if (streq (what, "user_info")) {
      chxtoken  (uid,  nullchix, 1, arg);
      ascofchx  (user, uid, 0L, USER_MAX-1);
      chxtoken  (name, nullchix, 2, arg);
      sprintf   (query, "SELECT %s FROM user_info WHERE userid='%s'",
                          ascquick(name), user);
      sql_quick (0, query, sqlbuf, 1);
      chxcat    (result, CQ(sqlbuf->str));
   }

   else if (streq (what, "userids_byid")) {
      chxtoken  (uid,  nullchix, 1, arg);
      ascofchx  (user, uid, 0L, USER_MAX-1);
   
      sprintf (query, "SELECT userid FROM user_info WHERE active >= %d", privValue("minimum"));
      type[0] = '\0';
      if (user[0])  sprintf (type, " AND userid LIKE '%s%%' ", user);
      strcat (query, type);
      strcat (query, " ORDER BY userid");

      append_query_results (query, result, sqlbuf);
   }

   else if (streq (what, "userids_byname")) {
      chxtoken  (temp, nullchix, 1, arg);
      if (! chxnum (temp, &cnum))  cnum = 0;

      strcpy (query,  "SELECT DISTINCT a2.userid FROM");
      strcpy (query2, " WHERE 1=1 ");
             
      for (num=2;   chxtoken (temp, nullchix, num, arg) >= 0;   ++num) {
         ascofchx (user, temp, 0L, USER_MAX-1);

         sprintf  (type, ", user_info a%d", num);
         strcat   (query, type);

         sprintf  (type, " AND concat("
             "' ', a%d.userid, ' ', ifnull(a%d.lname,' '), "
             "' ', ifnull(a%d.fname,' ')) LIKE '%% %s%%'", num, num, num, user);
         strcat   (query2, type);

         if (num > 2) {
            sprintf (type, " AND a2.userid = a%d.userid ", num);
            strcat  (query2, type);
         }
      }

      strcat   (query, query2);
      stralter (query, "FROM,", "FROM ");
    
      append_query_results (query, result, sqlbuf);
   }

   else if (streq (what, "set_user_info")) {
      chxtoken  (uid,  nullchix, 1, arg);
      ascofchx  (user, uid, 0L, USER_MAX-1);
      chxtoken  (name, nullchix, 2, arg);
      chxtoken  (nullchix, temp, 3, arg);
      chxclear  (temp2);
      func_escsingle (temp2, temp);
      ascofchx  (sqlbuf->str, temp2, 0L, sqlbuf->max-1);
      
      /*** set_user_info (user lname value) is the only choice that
      /    can INSERT (create) a new user; if it fails it tries UPDATE. 
      /    All other fields are UPDATE only. */
      rc     = 0;
      update = 1;
      ascii_buf (bigquery, strlen(sqlbuf->str) + 100);
      if (streq (ascquick(name), "lname")) {
         if (NULLSTR (sqlbuf->str))  rc = update = 0;
         else {
            sprintf (bigquery->str, 
               "INSERT INTO user_info (userid, lname, active, registeredon) "
               "VALUES ('%s', '%s', 10, now()) ", user, sqlbuf->str);
            rc = sql_quick_limited (0, bigquery->str, nothing, 100, 0);
            update = ! rc;
         }

         /*** Finish creating user.  This is temporary code during
         /    the transition from olde Caucus to MySQL. */
         if (rc) {
            if (sysudnum (uid, -1, 0) <= 0)  sysdirchk (uid);
         }
      }

      if (update) {
         ascofchx (ascname, name, 0L, USER_MAX-1);
         if (streq (ascname, "laston+")) {
            sysuserid (temp);
            if (chxeq (temp, uid))  set_laston();
         }
         else {
            sprintf  (bigquery->str,
                      "UPDATE user_info SET %s = '%s' WHERE userid='%s'",
                              ascname, sqlbuf->str, user);
            rc = sql_quick_limited (0, bigquery->str, nothing, 100, 0);
         }
      }
      chxcatval (result, THIN, (rc ? '1' : '0'));
   }

   /*** add_user_info (fieldname type readonly description) */
   else if (streq (what, "add_user_info")) {
      rc = chxtoken  (name, nullchix, 1, arg);
      ascofchx  (ascname, name, 0L, USER_MAX-1);
      rc = chxtoken  (temp, nullchix, 2, arg);
      ascofchx  (type,    temp, 0L, USER_MAX-1);
      if      (streq (type, "string"))  strcpy (type, "varchar(255)");
      else if (streq (type, "number"))  strcpy (type, "int");
      else if (streq (type, "text"))    strcpy (type, "mediumtext");
      else if (streq (type, "date"))    strcpy (type, "datetime");
      else if (streq (type, "dollars")) strcpy (type, "decimal(11,2)");
      else if (streq (type, "upload"))  strcpy (type, "varchar(255)");
      else                              rc = 0;
      rc = chxtoken  (temp, nullchix, 3, arg);
      ascofchx  (magic,    temp, 0L, USER_MAX-1);
      rc = chxtoken  (temp, nullchix, 4, arg);
      chxclear (temp2);
      func_escsingle (temp2, temp);
      ascofchx  (descr,    temp2, 0L, USER_MAX-1);

      if (rc > 0) {
         if (streq(type, "mediumtext")) {
            sprintf (query, "ALTER TABLE user_info ADD COLUMN (%s %s) ", ascname, type);
         }
         else {
            sprintf (query, "ALTER TABLE user_info ADD COLUMN (%s %s), "
                              "ADD INDEX (%s)", ascname, type, ascname);
         }
         rc = sql_quick_limited (0, query, nothing, 100, 0);
      }
      if (rc) {
         sprintf (query, "INSERT INTO user_columns (name, type, magic, descr) "
                         "VALUES ('%s', '%s', %s, '%s')", 
                         ascname, type, magic, descr);
         rc = sql_quick_limited (0, query, nothing, 100, 0);
      }
      chxcatval (result, THIN, (rc > 0 ? '1' : '0'));
   }

   /*** $USER_DATA(userid vname). */
   else if (streq (what, "user_data")) {
      rc = chxtoken  (uid,  nullchix, 1, arg);
      ascofchx (user,    uid,  0L, USER_MAX-1);
      rc = chxtoken  (name, nullchix, 2, arg);
      ascofchx (ascname, name, 0L, USER_MAX-1);
      if (rc > 0) {
         sprintf   (query, "SELECT value FROM user_data "
                           " WHERE userid='%s' AND name='%s'", user, ascname);
         rc = sql_quick (0, query, sqlbuf, 1);
         chxcat    (result, CQ(sqlbuf->str));
      }
   }

   /*** $LIST_USER_DATA(userid [vname]). */
   else if (streq (what, "list_user_data")) {
      rc = chxtoken  (uid,  nullchix, 1, arg);
      ascofchx (user, uid,  0L, USER_MAX-1);

      if (rc >= 0) {
         sprintf (bigquery->str, 
            "SELECT name FROM user_data WHERE userid='%s' ", user);

         rc = chxtoken (name, nullchix, 2, arg);
         ascofchx (ascname, name, 0L, USER_MAX-1);
         if (rc > 0) {
            sprintf (query, " AND name LIKE '%s%%' ", ascname);
            strcat  (bigquery->str, query);
         }
         strcat (bigquery->str, " ORDER BY name");

         append_query_results (bigquery->str, result, sqlbuf);
      }
   }

   /*** $SET_USER_DATA(userid vname value). */
   else if (streq (what, "set_user_data")) {
      rc = chxtoken  (uid,  nullchix, 1, arg);
      ascofchx (user,    uid,  0L, USER_MAX-1);
      rc = chxtoken  (temp, nullchix, 2, arg);
      chxclear (name);
      func_escsingle (name, temp);
      ascofchx (ascname, name, 0L, USER_MAX-1);
      found = chxtoken (nullchix, temp, 3, arg);
      chxclear (temp2);
      func_escsingle (temp2, temp);
      len = chxlen(temp2);
      ascii_buf (sqlbuf, len);
      ascofchx  (sqlbuf->str, temp2, 0L, sqlbuf->max);

      if (rc > 0) {
         ascii_buf (bigquery, len + 100);

         if (found >= 0) {
            sprintf (bigquery->str, 
               "INSERT INTO user_data (userid, name, value) "
               "  VALUES ('%s', '%s', '%s')",
               user, ascname, sqlbuf->str);
            rc = sql_quick_limited (0, bigquery->str, nothing, 10, 0);
   
            if (! rc) {
               sprintf   (bigquery->str, 
                  "UPDATE user_data SET value = '%s' "
                  " WHERE userid='%s' AND name='%s'",
                  sqlbuf->str, user, ascname);
               rc = sql_quick_limited (0, bigquery->str, nothing, 10, 0);
            }
         }
         else {
            sprintf   (bigquery->str, 
               "DELETE FROM user_data WHERE userid='%s' AND name='%s'", 
               user, ascname);
            rc = sql_quick_limited (0, bigquery->str, nothing, 10, 0);
         }
      }
      chxcatval (result, THIN, (rc ? '1' : '0'));
   }

   /*** $site_data(cnum inum vname) */
   else if (streq (what, "site_data")) {
      if (parse_site_data (arg, 3, ascCnum, ascInum, ascVname)) {
         sprintf (query, "SELECT value FROM site_data "
                         " WHERE name='%s' AND cnum=%s AND inum='%s'",
                  ascVname, ascCnum, ascInum);
         rc = sql_quick (0, query, sqlbuf, 1);
         chxcat    (result, CQ(sqlbuf->str));
      }
   }

   /*** $list_site_data (cnum inum vname-init) */
   else if (streq (what, "list_site_data")) {
      if (parse_site_data (arg, 2, ascCnum, ascInum, ascVname)) {
         sprintf (query, "SELECT name FROM site_data "
                         " WHERE cnum=%s AND inum='%s' AND name LIKE '%s%%'"
                         " ORDER BY name", 
                  ascCnum, ascInum, ascVname);
         append_query_results (query, result, sqlbuf);
      }
   }

   /*** $set_site_data (cnum inum vname values...) */
   else if (streq (what, "set_site_data")) {
      if (parse_site_data (arg, 2, ascCnum, ascInum, ascVname)) {

         /*** Parse out the 'values...', and escape any single quotes. */
         found = chxtoken (nullchix, temp, 4, arg);
         chxclear (temp2);
         func_escsingle (temp2, temp);
         len = chxlen(temp2);
         ascii_buf (sqlbuf, len);
         ascofchx  (sqlbuf->str, temp2, 0L, sqlbuf->max);

         ascii_buf (bigquery, len + 100);

         if (found >= 0) {
            sprintf (bigquery->str, 
               "INSERT INTO site_data (cnum, inum, name, value) "
               "  VALUES (%s, '%s', '%s', '%s')",
               ascCnum, ascInum, ascVname, sqlbuf->str);
            rc = sql_quick_limited (0, bigquery->str, nothing, 10, 0);
   
            if (! rc) {
               sprintf (bigquery->str, 
                  "UPDATE site_data SET value = '%s' "
                  " WHERE cnum=%s AND inum='%s' AND name='%s'",
                  sqlbuf->str, ascCnum, ascInum, ascVname);
               rc = sql_quick_limited (0, bigquery->str, nothing, 10, 0);
            }
         }
         else {
            sprintf (bigquery->str, 
               "DELETE FROM site_data "
               " WHERE cnum=%s AND inum='%s' AND name='%s'",
               ascCnum, ascInum, ascVname);
            rc = sql_quick_limited (0, bigquery->str, nothing, 10, 0);
         }
      }
      chxcatval (result, THIN, (rc ? '1' : '0'));
   }

   /*** $ITEM_VAR(cnum inum vname). */
   else if (streq (what, "item_var")) {
      if (touch_cv (ivar, arg, 1)) {
         chxtoken (name, nullchix, 3, arg);
         slot = vartab_find (ivar, name);
         chxcat (result, ivar->value[slot]);
      }
   }

   /*** $CLEAR_ITEM_VAR() */
   else if (streq (what, "clear_item_var")) {
      ivar->id1 = -1;
   }

   /*** $SET_ITEM_VAR(cnum inum vname value). */
   else if (streq (what, "set_item_var")) {
      if (touch_cv (ivar, arg, 1)) {
         chxtoken (name, nullchix, 3, arg);
         chxtoken (nullchix, temp, 4, arg);
         slot = vartab_find (ivar, name);
         if (NOT chxeq (temp, ivar->value[slot])) {
            chxcpy (ivar->value[slot], temp);
            ok = (a_chg_vartab (ivar, 0, A_WAIT) == A_OK);
         }
      }
   }

   /*** $LIST_ITEM_VAR(cnum inum vroot) */
   else if (streq (what, "list_item_var")) {
      if (touch_cv (ivar, arg, 1)) {
         empty = (chxtoken (name, nullchix, 3, arg) < 0);
         for (num=0;   num<ivar->used;   ++num) {
            if (empty  ||  chxindex (ivar->name[num], 0, name) == 0) {
               chxcat (result, ivar->name[num]);
               chxcatval (result, THIN, L(' '));
            }
         }
      }
   }


   /*** $SITE_VAR(vname). */
   else if (streq (what, "site_var")) {
      if (svar->id1 == SITE_CLEARED) {
         svar->id1 = -2;
         a_get_vartab (svar, 0, A_WAIT);
      }
      slot = vartab_find (svar, arg);
      chxcat (result, svar->value[slot]);
   }

   /*** $CLEAR_SITE_VAR() */
   else if (streq (what, "clear_site_var")) {
      svar->id1 = SITE_CLEARED;
   }

   /*** $SET_SITE_VAR(vname value). */
   else if (streq (what, "set_site_var")) {
      if (svar->id1 == SITE_CLEARED) {
         svar->id1 = -2;
         a_get_vartab (svar, 0, A_WAIT);
      }
      chxtoken (name, nullchix, 1, arg);
      chxtoken (nullchix, temp, 2, arg);
      slot = vartab_find (svar, name);
      if (NOT chxeq (temp, svar->value[slot])) {
         chxcpy (svar->value[slot], temp);
         ok = (a_chg_vartab (svar, 0, A_WAIT) == A_OK);
      }
   }

   /*** $LIST_SITE_VAR(cnum vroot) */
   else if (streq (what, "list_site_var")) {
      if (svar->id1 == SITE_CLEARED) {
         svar->id1 = -2;
         a_get_vartab (svar, 0, A_WAIT);
      }
      empty = (chxtoken (name, nullchix, 2, arg) < 0);
      for (num=0;   num<svar->used;   ++num) {
         if (empty  ||  chxindex (svar->name[num], 0, name) == 0) {
            chxcat (result, svar->name[num]);
            chxcatval (result, THIN, L(' '));
         }
      }
   }


   /*** $USER_VAR(userid vname). */
   else if (streq (what, "user_var")) {
      if (touch_uv (uvar, arg, name)) {
         chxtoken (name, nullchix, 2, arg);
         slot = vartab_find (uvar, name);
         chxcat (result, uvar->value[slot]);
      }
   }

   /*** $CLEAR_USER_VAR() */
   else if (streq (what, "clear_user_var")) {
      chxclear (uvar->owner);
   }

   /*** $SET_USER_VAR(userid vname value). */
   else if (streq (what, "set_user_var")) {
      if (touch_uv (uvar, arg, name)) {
         chxtoken (name, nullchix, 2, arg);
         chxtoken (nullchix, temp, 3, arg);
         slot = vartab_find (uvar, name);
         if (NOT chxeq (temp, uvar->value[slot])) {
            chxcpy (uvar->value[slot], temp);
            code = a_chg_vartab (uvar, 0, A_WAIT);
            ok = (code == A_OK);
         }
      }
   }

   /*** $LIST_USER_VAR(userid vroot) */
   else if (streq (what, "list_user_var")) {
      touch_uv (uvar, arg, name);
      empty = (chxtoken (name, nullchix, 2, arg) < 0);
      for (num=0;   num<uvar->used;   ++num) {
         if (empty  ||  chxindex (uvar->name[num], 0, name) == 0) {
            chxcat (result, uvar->name[num]);
            chxcatval (result, THIN, L(' '));
         }
      }
   }


   RETURN (1);
}


FUNCTION  touch_cv (var, arg, item)
   Vartab var;
   Chix   arg;
   int    item;
{
   int4   pos, cnum;
   int    ok, inum;
   ENTRY ("touch_cv", "");

   ok   = 1;
   pos  = L(0);
   cnum = chxint4 (arg, &pos);
   inum = chxint4 (arg, &pos);
   if (cnum <= 0)  return (0);
   if (cnum != var->id1  ||  (item && inum!=var->id2)) {
      var->id1 = cnum;
      var->id2 = (item ? inum : 0);
      ok = (a_get_vartab (var, 0, A_WAIT) != A_BADARG);
   }

   RETURN (ok);
}

FUNCTION  touch_uv (uvar, arg, word)
   Vartab uvar;
   Chix   arg, word;
{
   int    ok;
   ENTRY ("touch_uv", "");

   ok = 1;
   chxtoken (word, nullchix, 1, arg);
   if (NOT chxeq (uvar->owner, word)) {
      chxcpy (uvar->owner, word);
      ok = (a_get_vartab (uvar, 0, A_WAIT) != A_BADARG);
   }

   RETURN (ok);
}

/*** Parse arguments to site_data(), list_site_data(), set_site_data().
/    Returns 1 if got # of args required, else 0. */
FUNCTION int parse_site_data (Chix arg, int required, 
                              char *ascCnum, char *ascInum, char *ascVname) {
   char  *p[4];
   static Chix temp = nullchix;
   static Chix temp2;
   int    i;

   if (temp == nullchix) {
      temp  = chxalloc (L(40), THIN, "parse_site temp");
      temp2 = chxalloc (L(40), THIN, "parse_site temp2");
   }

   p[1] = ascCnum;
   p[2] = ascInum;
   p[3] = ascVname;
   for (i=1;   i<=3;   ++i) p[i][0] = '\0';

   for (i=1;   i<=3;   ++i) {
      if (chxtoken (temp, nullchix, i, arg) < 0)  return (i > required);
      chxclear (temp2);
      func_escsingle (temp2, temp);
      ascofchx (p[i], temp2, 0, USER_MAX);
   }
   return (1);
}

/*** Append results of a query, blank-separated, to chix. */
FUNCTION int append_query_results (char *query, Chix result, AsciiBuf sqlbuf) {
   int  rc;

   rc = sql_exec (0, query);
   if (rc == SQL_SUCCESS) {
      while ( (rc = sql_fetch(0)) != SQL_NO_DATA) {
         while ( (rc = sql_getchardata (0, sqlbuf)) != SQL_NO_DATA) {
            chxcat    (result, CQ(sqlbuf->str));
            chxcatval (result, THIN, L(' '));
         }
      }
   }
   return (1);
}
