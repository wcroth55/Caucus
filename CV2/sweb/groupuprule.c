
/*** GROUP_UPDATE_RULE.  Update groups table after changing a single rule
/                        in a single group.
/
/    ok = group_update_rule (changer, changed);
/ 
/    Parameters:
/       Chix  changer;   (subject of rule change: userid, wildcard,
/                         $quote()'d group name, or $quote()'d "key keynumber")
/       Chix  changed;   (group name: $quote()'d value of "owner name" pair)
/
/    Purpose:
/       When a single grouprule in a group is changed, perform the
/       optimal (fastest) updates to the 'groups' table... by rebuilding
/       only that part of 'groups' that could have been changed for the
/       users matched by 'changer'.  Carry this up the chain
/       as needed for groups that include the 'changed' group, thru
/       any number of levels of inclusion.
/
/    How it works:
/       Note that a group is actually two words: the owner
/       (userid, "CONF", "MGR", etc.) and then a group name.
/
/       The parsing of grouprules rows into groups rows happens in
/       N phases.  The groups.active column defines the state of
/       a row: 1=>"real", 0=>prior to min/max access filtering,
/              3=>after filtering but before removal of old active=1 rows.
/
/       Parsing phases: (all apply only to the current group)
/          1. Each grouprules rule adds one or more rows to groups
/             with the respective access, with active=0.  Each row
/             is assigned a priority:
/                exclude rows get priority 99
/                bySelf rows get:
/                   (optional -- priority 50 + access/10 -- e.g. self resigning)
/                   (non-optional -- priority + 1 to advance over wildcards)
/                all other rows get priority = access
/          2. The active=0 rows are scanned.  If there are more than one
/             for a given userid:
/                (a) the highest priority one is copied, with active=3.
/                (b) all rows for this userid are remembered for deletion.
/          3. Rows from 2b are deleted.
/          4. All active=1 rows are removed.
/          5. All active=3 or active=0 rows are changed to active=1.
/
/    Returns:
/
/    Error Conditions:
/
/    Notes -- ways to improve speed:
/       1. Handle wildcards of "%" differently, by simplifying the queries.
/          (Or does the MySQL optimizer do that for us already?)
/
/       2. For anything other than "%" or a single userid match, putting
/          the list of matching userids into a temporary table, and then
/          just doing a join against that.
/ 
/    Home:  groupupdate.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/20/05 New function, seriously hacked from group_update().  */
/*: CR 11/28/05 Make sure MAX(priority) selection only gets one row! */
/*: CR 01/22/06 bySelf non-optional rows get priority+1. */
/*: CR 03/06/06 Rewrite MAX(priority) section. */
/*: CR 04/11/07 Break long list of deleteRows up into 2K chunks, it seems
/     SQLExecDirect has a size limit! */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"
#include "sqlcaucus.h"
#include "sql.h"

#define  STRMAX 600
#define  QMAX  2000

extern AsciiBuf  sqlbuf;

FUNCTION  group_update_rule (Chix changer, Chix changed) {

   static Chix todo = nullchix;
   static Chix done, thisowner, thisname, temp, word1, word2, deleteRows, deleteChunk;
   char        name[STRMAX], owner[STRMAX], rowname[STRMAX], rowowner[STRMAX];
   char        userid[STRMAX], wildcard[STRMAX], subowner[STRMAX],
               subname[STRMAX], access[STRMAX], matchuser[STRMAX],
               optional[STRMAX], byself[20],
               maxUserid[STRMAX], maxAccess[STRMAX], maxPriority[STRMAX],
               prvUserid[STRMAX], prvAccess[STRMAX], prvPriority[STRMAX];
   char        chgid[STRMAX], chgwild[STRMAX], chgkey[STRMAX],
               chgowner[STRMAX], chgname[STRMAX];
   char        subgroup[STRMAX+STRMAX], strtemp[STRMAX];
   char        query[QMAX],  query2[QMAX], nothing[10];
   int         rc0, rc1, rc2, rc3, is_optional, access_val, priority,
               rowsFound, last, pos;
   int getNextChunk (Chix chunk, Chix original, int pos, int chunkSize, char delimiter);

   ENTRY ("group_update", "");

   /*** Initialization. */
   if (todo == nullchix) {
      todo       = chxalloc     (L( 40), THIN, "func_var todo");
      done       = chxalloc     (L( 40), THIN, "func_var done");
      thisowner  = chxalloc     (L( 40), THIN, "func_var thisowner");
      thisname   = chxalloc     (L( 40), THIN, "func_var thisname");
      temp       = chxalloc     (L( 40), THIN, "func_var temp");
      word1      = chxalloc     (L( 40), THIN, "func_var word1");
      word2      = chxalloc     (L( 40), THIN, "func_var word2");
      deleteRows = chxalloc     (L(400), THIN, "func_var deleteRows");
      deleteChunk = chxalloc    (L(400), THIN, "func_var deleteChunk");
      strcpy (query, "CREATE TEMPORARY TABLE temp_ids (userid varchar(200))");
      rc0 = sql_quick_limited (0, query, nothing, 9, 0);
   }

   /*** Parse out the subject (changer) of the change: userid, wildcard,
   /    or subgroup */
   chxcpy    (temp, changer);
   chxrepval (temp, 0, THIN, 1, 32);
   chxtoken  (word1, nullchix, 1, temp);
   chxtoken  (word2, nullchix, 2, temp);
   chgid[0] = chgwild[0] = chgowner[0] = chgname[0] = chgkey[0] = '\0';
   if      (chxlen(word2) > 0) {
      ascofchx  (chgowner, word1, 0, STRMAX);
      ascofchx  (chgname,  word2, 0, STRMAX);
   }
   else if (chxindex (word1, 0, CQ("%")) >= 0) {
      ascofchx  (chgwild, word1, 0, STRMAX);
   }
   else if (chxlen (word1) > 0) {
      ascofchx  (chgid,   word1, 0, STRMAX);
   }
   else RETURN (0);

   /*** Handle "key #" case. */
   if (streq (chgowner, "key")) {
      sprintf (query,
         "SELECT userid, wildcard, subowner, subname FROM grouprules "
         " WHERE grkey=%s", chgname);
      rc0 = (sql_exec (0, query) == SQL_SUCCESS);
      chgid[0] = chgowner[0] = wildcard[0] = '\0';
      while (rc0 && (sql_fetch(0) != SQL_NO_DATA)) {
         sql_getcharlimited (0, chgid,    STRMAX);
         sql_getcharlimited (0, wildcard, STRMAX);
         sql_getcharlimited (0, chgowner, STRMAX);
         sql_getcharlimited (0, chgname,  STRMAX);
      }
      if (wildcard[0] == '1') { strcpy (chgwild, chgid);   chgid[0] = '\0'; }
   }

   chxcpy   (todo, changed);
   chxclear (done);

   /*** For each group remaining in our "todo" list... */
   while (chxtoken (temp, nullchix, 1, todo) >= 0) {

      /*** Add it to "done". */
      chxcatval (done, THIN, L(' '));
      chxcat    (done, temp);

      /*** Pluck out the owner/name parts. */
      chxrepval (temp, 0, THIN, 1, 32);
      chxtoken  (thisowner, nullchix, 1, temp);
      chxtoken  (thisname,  nullchix, 2, temp);
      ascofchx  (owner, thisowner, 0, STRMAX);
      ascofchx  (name,  thisname,  0, STRMAX);

      /*** Remove this group from "todo". */
      chxtoken  (nullchix, temp, 2, todo);
      chxcpy    (todo, temp);

      /*** Now examine each row in the grouprules for this group, and
      /    insert the appropriate rows in the groups table.  Later we'll
      /    filter out only the ones we want to keep. */
      sprintf (query,
         "SELECT userid, wildcard, subowner, subname, access, optional, bySelf "
         "  FROM grouprules "
         " WHERE owner='%s' AND name='%s' ", owner, name);
      if (chgid[0]) {
         sprintf (query2,
            " AND (userid = '%s'  "
            "  OR (wildcard AND '%s' LIKE userid) "
            "  OR (subowner is NOT NULL  AND  subowner != ''))", chgid, chgid);
         strcat (query, query2);
      }

      rc0 = (sql_exec (0, query) == SQL_SUCCESS);
      while (rc0 && (sql_fetch(0) != SQL_NO_DATA)) {
         sql_getcharlimited (0, userid,   STRMAX);
         sql_getcharlimited (0, wildcard, STRMAX);
         sql_getcharlimited (0, subowner, STRMAX);
         sql_getcharlimited (0, subname,  STRMAX);
         sql_getcharlimited (0, access,   STRMAX);
         sql_getcharlimited (0, optional, STRMAX);
         sql_getcharlimited (0, byself,   20);

         /*** Optional rules get get access = 1/10 of actual access,
         /    to record opt-up availability later.  Delete rules get
         /    high priority (just under primary).  Optional bySelf
         /    rules (such as a resign) get priority > organizer, so that
         /    person can always resign. */
         is_optional = atoi (optional);
         access_val  = atoi (access);
         if (is_optional) sprintf (access, "%d", access_val / 10);
         priority = access_val;
         if (priority == 0)  priority = 99;
         if (atoi(byself)) {
            if (is_optional)   priority = 50 + access_val/10;
            else               priority++;
         }

         /*** Group X including itself is illegal! */
         if (streq (owner, subowner)  &&  streq (name, subname))  continue;

         /*** Single userid or group in rule. */
         if (userid[0]  &&  wildcard[0] == '0') {

            if (chgid[0]  &&  streq (userid, chgid)) {
               sprintf (query, "INSERT INTO groups "
                  "    (userid, owner, name, access, active, priority) "
                  "    VALUES ('%s', '%s', '%s', %s, 0, %d)",
                        userid, owner, name, access, priority);
            }
            else if (chgwild[0]) {
               sprintf (query,
                  "INSERT INTO groups "
                  "    (userid, owner, name, access, active, priority) "
                  "    SELECT a.userid, '%s', '%s', %s, 0, %d  "
                  "      FROM (SELECT '%s' userid) a "
                  "     WHERE a.userid LIKE '%s' ",
                  owner, name, access, priority, userid, chgwild);
            }
            else if (chgowner[0]) {
               sprintf (query,
                  "INSERT INTO groups "
                  "    (userid, owner, name, access, active, priority) "
                  "    SELECT g.userid, '%s', '%s', %s, 0, %d  "
                  "      FROM groups g "
                  "     WHERE g.userid = '%s' AND g.active=1 "
                  "       AND g.owner='%s'    AND g.name='%s' ",
                  owner, name, access, priority, userid, chgowner, chgname);
            }
            rc1 = sql_quick_limited (1, query, nothing, 9, 0);
            glog (rc1, query);
         }
  
         /*** Userid wildcard, find all matching and insert a row for each. */
         else if (userid[0]  &&  wildcard[0] == '1') {

            if (chgid[0]) {
               sprintf (query,
                  "INSERT INTO groups "
                  "    (userid, owner, name, access, active, priority) "
                  "    SELECT a.userid, '%s', '%s', %s, 0, %d  "
                  "      FROM (SELECT '%s' userid) a "
                  "     WHERE a.userid LIKE '%s' ",
                  owner, name, access, priority, chgid, userid);
            }
            else if (chgwild[0]) {
               sprintf (query,
                  "INSERT INTO groups "
                  "    (userid, owner, name, access, active, priority) "
                  "    SELECT userid, '%s', '%s', %s, 0, %d  "
                  "      FROM user_info WHERE active > 0 "
                  "       AND userid LIKE '%s'  AND  userid LIKE '%s'",
                  owner, name, access, priority, userid, chgwild);
            }
            else if (chgowner[0]) {
               sprintf (query,
                  "INSERT INTO groups "
                  "    (userid, owner, name, access, active, priority) "
                  "    SELECT userid, '%s', '%s', %s, 0, %d  "
                  "      FROM groups "
                  "     WHERE owner='%s' AND name='%s' "
                  "       AND active=1   AND userid LIKE '%s'",
                  owner, name, access, priority, chgowner, chgname, userid);
            }

            rc1 = sql_quick_limited (1, query, nothing, 9, 0);
            glog (rc1, query);
         }

         /*** Subgroup, with inherit access.  Find every row in 'groups'
         /    for the subgroup, and insert it! */
         else if (streq (access, "-1")) {
            if (chgid[0]) {
               sprintf (query, "INSERT INTO groups "
                  " (userid, access, owner, name, active, priority) "
                  " SELECT userid, access, '%s', '%s', 0, access FROM groups"
                  "  WHERE userid='%s' "
                  "    AND  owner='%s' AND name='%s' AND active = 1",
                  owner, name, chgid, subowner, subname);
            }
            else if (chgwild[0]) {
               sprintf (query, "INSERT INTO groups "
                  " (userid, access, owner, name, active, priority) "
                  " SELECT userid, access, '%s', '%s', 0, access FROM groups"
                  "  WHERE userid LIKE '%s' "
                  "    AND  owner='%s' AND name='%s' AND active = 1",
                  owner, name, chgwild, subowner, subname);
            }
            else if (chgowner[0]) {
               sprintf (query, "INSERT INTO groups "
                  " (userid, access, owner, name, active, priority) "
                  " SELECT DISTINCT a.userid, a.access, '%s', '%s', 0, a.access"
                  "   FROM groups a, groups b "
                  "  WHERE a.userid = b.userid "
                  "    AND a.owner='%s' AND a.name='%s' AND a.active = 1 "
                  "    AND b.owner='%s' AND b.name='%s' AND b.active = 1 ",
                  owner, name, subowner, subname, chgowner, chgname);
            }

            rc1 = sql_quick_limited (1, query, nothing, 9, 0);
            glog (rc1, query);
         }

         /*** Subgroup, with specific access.  (Do not include users
         /    with "optional" access to the subgroup.) */
         else {
            if (chgid[0]) {
               sprintf (query, "INSERT INTO groups "
                  " (userid, access, owner, name, active, priority) "
                  " SELECT userid, %s, '%s', '%s', 0, %d FROM groups"
                  "  WHERE userid='%s' "
                  "    AND  owner='%s'   AND name='%s' "
                  "    AND  access >=10  AND active = 1",
                  access, owner, name, priority, chgid, subowner, subname);

            }
            else if (chgwild[0]) {
               sprintf (query, "INSERT INTO groups "
                  " (userid, access, owner, name, active, priority) "
                  " SELECT userid, %s, '%s', '%s', 0, %d FROM groups "
                  "  WHERE userid LIKE '%s' "
                  "    AND owner='%s' AND name='%s' AND access >= 10 "
                  "    AND active = 1",
                  access, owner, name, priority, chgwild, subowner, subname);
            }
            else if (chgowner[0]) {
               sprintf (query, "INSERT INTO groups "
                  " (userid, access, owner, name, active, priority) "
                  " SELECT DISTINCT a.userid, %s, '%s', '%s', 0, %d "
                  "   FROM groups a, groups b "
                  "  WHERE a.userid = b.userid "
                  "    AND a.owner='%s' AND a.name='%s' AND a.active = 1 "
                  "    AND b.owner='%s' AND b.name='%s' AND b.active = 1 "
                  "    AND a.access >= 10 ",
                  access, owner, name, priority, subowner, subname,
                  chgowner, chgname);
            }

            rc1 = sql_quick_limited (1, query, nothing, 9, 0);
            glog (rc1, query);
         }

      }


      /*** Maximum priority access rules win.   See phases 2 a&b, 3, above. */
      sprintf (query,
         "SELECT userid, access, priority FROM groups "
         " WHERE owner='%s' AND name='%s' AND active=0 "
         " ORDER BY userid, priority DESC", owner, name);

      chxclear (deleteRows);
      prvUserid[0] = '\0';
      rowsFound    =   1;
      rc0 = (sql_exec (0, query) == SQL_SUCCESS);
      while (rc0 && (sql_fetch(0) != SQL_NO_DATA)) {
         sql_getcharlimited (0, maxUserid,   STRMAX);
         sql_getcharlimited (0, maxAccess,   STRMAX);
         sql_getcharlimited (0, maxPriority, STRMAX);

         /*** If we find a second row for the same userid, re-insert
         /    the first row (with active=3), and then mark *all* active=0
         /    rows for this userid for deletion. */
         if (streq(maxUserid, prvUserid)) {
            if (++rowsFound == 2) {
               sprintf (query2,
                  "INSERT INTO groups (userid, owner, name, access, active, "
                  "  priority) VALUES ('%s', '%s', '%s', %s, 3, %s)",
                  prvUserid, owner, name, prvAccess, prvPriority);
               rc1 = sql_quick_limited (1, query2, nothing, 9, 0);
               glog (rc1, query2);

               /*** Add prvUserid to list of active=0 rows to delete. */
               sprintf (strtemp, ",'%s'", prvUserid);
               chxcat  (deleteRows, CQ(strtemp));
            }
         }
         else rowsFound = 1;

         strcpy (prvUserid,   maxUserid);
         strcpy (prvAccess,   maxAccess);
         strcpy (prvPriority, maxPriority);
      }

      /*** Delete all the phase 2b rows. */
      for (pos=0;   (pos = getNextChunk (deleteChunk, deleteRows, pos, 1000, ',')) >= 0; ) {
         ascii_buf (sqlbuf, chxlen(deleteChunk) + 100);

         sprintf   (sqlbuf->str,
            "DELETE FROM groups WHERE owner='%s' AND name='%s' AND active=0 "
            "   AND userid IN (''", owner, name);

         last = strlen (sqlbuf->str);
         ascofchx (sqlbuf->str + last, deleteChunk, 0, ALLCHARS);
         strcat   (sqlbuf->str, ")");
         rc0 = sql_quick_limited (0, sqlbuf->str, nothing, 9, 0);
         glog (rc1, sqlbuf->str);
      }
  
      /*** Delete all the old (active=1) entries. */
      if (chgid[0]) {
         sprintf (query, "DELETE FROM groups "
            " WHERE owner='%s' AND name='%s' AND userid='%s' "
            "   AND active = 1", owner, name, chgid);
      }
      else if (chgwild[0]) {
         sprintf (query, "DELETE FROM groups "
            " WHERE owner='%s' AND name='%s' AND userid LIKE '%s' "
            "   AND active = 1", owner, name, chgwild);
      }
      else if (chgowner[0]) {
         rc0 = sql_quick_limited (0, "DELETE FROM temp_ids", nothing, 9, 0);
         sprintf (query,
            " INSERT INTO temp_ids (userid) "
            "    SELECT DISTINCT userid FROM groups "
            "     WHERE owner='%s' AND name='%s' AND active=1",
            chgowner, chgname);
         rc0 = sql_quick_limited (0, query, nothing, 9, 0);
         glog (rc0, query);
  
         sprintf (query, "DELETE FROM groups "
            " WHERE owner='%s' AND name='%s' AND active = 1"
            "   AND userid IN (SELECT userid FROM temp_ids)",
            owner, name);
      }

      rc0 = sql_quick_limited (0, query, nothing, 9, 0);
      glog (rc0, query);

      /*** Mark the new entries as standard. */
      sprintf (query,
         "UPDATE groups SET active = 1 "
         " WHERE owner='%s' AND name='%s' AND active IN (0, 3)",
         owner, name);
      rc0 = sql_quick_limited (0, query, nothing, 9, 0);
      glog (rc0, query);

      /*** Find all groups that include *this* group and are NOT in "todo"
      /    or "done", and add them to "todo". */
      sprintf (query,
         "SELECT DISTINCT owner, name FROM grouprules "
         " WHERE subowner='%s' AND subname='%s' ",
         owner, name);
      rc0 = (sql_exec (0, query) == SQL_SUCCESS);
      while (rc0 && (sql_fetch(0) != SQL_NO_DATA)) {
         sql_getcharlimited (0, owner, STRMAX);
         sql_getcharlimited (0, name,  STRMAX);
         sprintf   (subgroup, "%s %s", owner, name);
         chxcpy    (temp, CQ(subgroup));
         chxrepval (temp, 0, THIN, 32, 1);
         if (tablematch (todo, temp) == -1  &&
             tablematch (done, temp) == -1) {
            chxcatval (todo, THIN, L(' '));
            chxcat    (todo, temp);
         }
      }
   }

   /*** Force effective clearing of group access cache info. */
   group_cache_age (time(NULL));
   RETURN (1);
}

#define GLOG_MAX 2000
FUNCTION glog (int rc, char *query) {
   char  temp[GLOG_MAX];

   sprintf (temp, "glog: rc=%d, query='%s'", rc, query);

   sprintf (temp, "glog: rc=%d, query=", rc);
   strncat (temp, query, GLOG_MAX - 4);
   logger  (0, LOG_FILE, temp);
}

/*** Get the next "chunk" (of a certain size) of a set of delimited tokens.
/    (We need this because some SQL ODBC functions can only handle up to
/    a certain size query.)
/ */
FUNCTION int getNextChunk (Chix chunk, Chix original, int pos, int chunkSize,
                           char delimiter) {
   int   origLen, newPos;

   chxclear (chunk);
   origLen = chxlen(original);
  
   if (pos >= origLen)  return -1;

   newPos = pos + chunkSize;
   if (newPos >= origLen) {
      chxcatsub (chunk, original, pos, ALLCHARS);
      return origLen;
   }

   for (;   newPos > pos;   --newPos) {
      if (chxvalue (original, newPos) == delimiter) {
         chxcatsub (chunk, original, pos, (newPos - pos));
         return newPos;
      }
   }

}

