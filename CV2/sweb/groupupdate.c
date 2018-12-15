
/*** GROUP_UPDATE.  Update groups table after changing a single group.
/
/    ok = group_update (changed);
/   
/    Parameters:
/       Chix  changed;   (group name: $quote()'d value of "owner name" pair)
/
/    Purpose:
/       When the grouprules for a single group have been changed,
/       update the groups table for that group *and* any groups
/       that include that group, through any number of levels of inclusion.
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
/                bySelf  rows get priority 50 + access/10
/                all other rows get priority = access
/          2. A new row is added (for each userid) with the maximum 
/             priority value of all rows (for this userid), with active=3.
/          3. All active=0 and active=1 rows are removed.
/          4. All active=3 rows are changed to active=1.
/
/    Returns: 0 if bad arguments, 1 otherwise.
/
/    Error Conditions:
/  
/    Home:  groupupdate.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/17/05 New function. */
/*: CR 10/27/05 Use DISTINCT when selecting maximum priority entries. */
/*: CR 11/07/05 Only *optional* bySelf rules get higher priority. */
/*: CR 11/20/05 Comment out table group_ids, add return code. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"
#include "sqlcaucus.h"
#include "sql.h"

#define  STRMAX 600
#define  QMAX  2000

extern AsciiBuf  sqlbuf;

FUNCTION  group_update (Chix changed) {

   static Chix todo = nullchix;
   static Chix done, thisowner, thisname, temp;
   char        name[STRMAX], owner[STRMAX], rowname[STRMAX], rowowner[STRMAX];
   char        userid[STRMAX], wildcard[STRMAX], subowner[STRMAX],
               subname[STRMAX], access[STRMAX], matchuser[STRMAX],
               optional[STRMAX], byself[20];
   char        subgroup[STRMAX+STRMAX];
   char        query[QMAX],  nothing[10];
   int         rc0, rc1, rc2, rc3, is_optional, access_val, priority;
   int         success;

   ENTRY ("group_update", "");

   /*** Initialization. */
   if (todo == nullchix) {
      todo      = chxalloc     (L(40), THIN, "func_var todo");
      done      = chxalloc     (L(40), THIN, "func_var done");
      thisowner = chxalloc     (L(40), THIN, "func_var thisowner");
      thisname  = chxalloc     (L(40), THIN, "func_var thisname");
      temp      = chxalloc     (L(40), THIN, "func_var temp");

/*
      strcpy (query, "CREATE TEMPORARY TABLE group_ids (userid varchar(200))");
      rc0 = sql_quick_limited (0, query, nothing, 9, 0);
*/
   }

   chxcpy   (todo, changed);
   chxclear (done);

   /*** For each group remaining in our "todo" list... */
   success = 0;
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
      if (NOT name[0])  break;
      success = 1;

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
         if (atoi(byself)  &&  is_optional)   
            priority = privValue("organizer") + 10 + access_val/10; 

         /*** Group X including itself is illegal! */
         if (streq (owner, subowner)  &&  streq (name, subname))  continue;

         /*** Single userid, just insert groups row. */
         if (userid[0]  &&  wildcard[0] == '0') {
            sprintf (query, 
               "INSERT INTO groups "
               "   (userid, owner, name, access, active, priority) "
               "   VALUES ('%s', '%s', '%s', %s, 0, %d)", 
               userid, owner, name, access, priority);
            rc1 = sql_quick_limited (1, query, nothing, 9, 0);
         }
   
         /*** Userid wildcard, find all matching and insert a row for each. */
         else if (userid[0]  &&  wildcard[0] == '1') {
            sprintf (query, 
               "INSERT INTO groups "
               "   (userid, access, owner, name, active, priority) "
                   "SELECT userid, %s, '%s', '%s', 0, %d FROM user_info "
                   " WHERE userid LIKE '%s'", 
               access, owner, name, priority, userid);
            rc1 = sql_quick_limited (1, query, nothing, 9, 0);
         }

         /*** Subgroup, with inherit access.  Find every row in 'groups'
         /    for the subgroup, and insert it! */
         else if (streq (access, "-1")) {
            sprintf (query, 
               "INSERT INTO groups "
               "    (userid, access, owner, name, active, priority) "
                   "SELECT userid, access, '%s', '%s', 0, access FROM groups "
                   " WHERE owner='%s' AND name='%s' AND active = 1",
               owner, name, subowner, subname);
            rc1 = sql_quick_limited (1, query, nothing, 9, 0);
         }

         /*** Subgroup, with specific access.  (Do not include users
         /    with "optional" access to the subgroup.) */
         else {
            sprintf (query, 
               "INSERT INTO groups "
                   " (userid, access, owner, name, active, priority) "
                   "SELECT userid, %s, '%s', '%s', 0, %d FROM groups "
                   " WHERE owner='%s' AND name='%s' AND active = 1 "
                   " AND access >= %d",
               access, owner, name, priority, subowner, subname, privValue("minimum"));
            rc1 = sql_quick_limited (1, query, nothing, 9, 0);
         }

      }


      /*** Maximum priority access rules win.  Make a copy of all the
      /    maximum priority entries, as active=3. */
/*
      sprintf (query, 
         "INSERT INTO groups (userid, access, owner, name, active, priority) "
         "   SELECT userid, access, '%s', '%s', 3, MAX(priority) FROM groups "
         "    WHERE owner='%s' AND name='%s' AND active = 0 GROUP BY userid ",
         owner, name, owner, name);
*/

      sprintf (query, 
         "INSERT INTO groups (userid, access, owner, name, active, priority) "
         "   SELECT DISTINCT g.userid, g.access, '%s', '%s', 3, g.priority "
         "   FROM groups g"
         "    WHERE g.owner='%s' AND g.name='%s' AND g.active = 0 "
         "      AND g.priority = "
         "          (SELECT MAX(priority) FROM groups "
         "            WHERE userid=g.userid AND owner=g.owner "
         "              AND name=g.name AND active=0) ",
              owner, name, owner, name);
      rc0 = sql_quick_limited (0, query, nothing, 9, 0);
/*
      { char temp[200];
        fprintf (stderr, "query=%s\n\n", query);
        fprintf (stderr, "rc=%d\n", rc0);
        gets (temp);
      }
*/


      /*** Delete all the old entries. */
      sprintf (query,
         "DELETE FROM groups WHERE owner='%s' AND name='%s' AND active IN (0,1) ",
         owner, name);
      rc0 = sql_quick_limited (0, query, nothing, 9, 0);

      /*** Mark the new entries as standard. */
      sprintf (query, 
         "UPDATE groups SET active = 1 WHERE owner='%s' AND name='%s'",
         owner, name);
      rc0 = sql_quick_limited (0, query, nothing, 9, 0);

      /*** Find all groups that include *this* group and are NOT in "todo"
      /    or "done", and add them to "todo". */
      sprintf (query, 
         "SELECT DISTINCT owner, name FROM grouprules "
         " WHERE subowner='%s' AND subname='%s' AND NOT optional",
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
   RETURN (success);
}
