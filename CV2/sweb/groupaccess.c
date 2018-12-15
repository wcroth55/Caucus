
/*** GROUP_ACCESS.  Return access level of user in group.
/
/    access = group_access (userid, gtype, gname);
/    access = conf_access  (cnum);
/   
/    Parameters:
/       userid
/       gtype    (group type, e.g. "MGR", "CONF", etc.)
/       gname    (group name)
/       cnum     (conference number)
/
/    Purpose:
/       Return access level of a userid to a group.  Caches values
/       for up to CACHE_AGE seconds, to provide fast evaluation.
/
/    How it works:
/
/    Returns: access level (e.g. 40 is organizer, 0 is no access, etc.)
/
/    Error Conditions:
/  
/    Home:  groupaccess.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/30/05 New function. */
/*: CR  4/16/07 Don't use [slot] until we know it's >= 0. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "sqlcaucus.h"
#include "sql.h"

#define  STRMAX 100
#define  CACHE_SIZE  40
#define  CACHE_AGE  100

FUNCTION  group_access (Chix userid, Chix gtype, Chix gname) {
   static Chix cache_name[CACHE_SIZE];
   static int  cache_time[CACHE_SIZE];
   static int  cache_used[CACHE_SIZE] = { -1 };
   static int  cache_valu[CACHE_SIZE];
   static Chix group;
   int         i, slot, zero, used, uage, rc, now, value, is_current;
   char        query[1000], 
               suserid[STRMAX], sgtype[STRMAX], sgname[STRMAX], buf[STRMAX];

   ENTRY ("group_access", "");

   /*** Initialization. */
   if (cache_used[0] < 0) {
      for (i=0;   i<CACHE_SIZE;   ++i) {
         cache_name[i] = chxalloc (L(40), THIN, "group_access cache_name");
         cache_time[i] = 0;
         cache_used[i] = 0;
      }
      group = chxalloc (L(40), THIN, "group_access group");
   }

   chxcpy    (group, userid);
   chxcatval (group, THIN, L('.'));
   chxcat    (group, gtype);
   chxcatval (group, THIN, L('.'));
   chxcat    (group, gname);

   /*** See if group is in cache. */
   slot = -1;
   zero = -1;    /* Find first empty slot */
   used =  0;    /* Find oldest (least recently used) slot */
   uage = cache_used[0];
   for (i=0;   i<CACHE_SIZE;   ++i) {
      if (chxeq (group, cache_name[i]))     { slot = i;  break; }
      if (zero < 0  &&  cache_time[i] == 0) { zero = i;  break; }
      if (cache_used[i] < uage)             { used = i;  uage = cache_used[i]; }
   }

   /*** Found group in cache, and the value is current. */
   now = time(NULL);
   if (slot >= 0) {
      is_current = cache_time[slot] > max (now - CACHE_AGE, group_cache_age(0));
      if (is_current) {
         cache_used[slot] = now;
         RETURN (cache_valu[slot]);
      }
   }

   /*** If not found, get an empty slot, or else the least-recently-used
   /    slot, and use it for this group. */
   if (slot < 0) {
      if (zero >= 0)   slot = zero;   
      else             slot = used;
      chxcpy (cache_name[slot], group);
   }


   /*** Here if not found, or out of date.  Update with current value. */
   ascofchx (suserid, userid, 0, STRMAX);
   ascofchx (sgtype,  gtype,  0, STRMAX);
   ascofchx (sgname,  gname,  0, STRMAX);
   sprintf  (query, 
      "SELECT MAX(access) FROM groups "
      " WHERE userid='%s' AND owner='%s' AND name='%s'",
      suserid, sgtype, sgname);
   value = 0;  /* used to be -2 for some reason, probably bug. 8/31/09 */
   if (sql_quick_limited (0, query, buf, STRMAX, 1)  &&
       strlen (buf) > 0)                      value = atoi(buf);
   cache_valu[slot] = value;
   now = time(NULL);
   cache_used[slot] = now;
   cache_time[slot] = now;
   RETURN (value);
}

FUNCTION int conf_access (int cnum) {
   static Chix uid = nullchix;
   static Chix conf, confnum;
   char        temp[20];

   if (uid == nullchix) {
      uid     = chxalloc (L(40), THIN, "conf_access uid");
      confnum = chxalloc (L(40), THIN, "conf_access confnum");
      conf    = chxalsub (CQ("CONF"), L(0), L(4));
   }
      
   sysuserid  (uid);
   sprintf    (temp, "%d", cnum);
   chxofascii (confnum, temp);

   return (group_access (uid, conf, confnum));
}

FUNCTION int group_cache_age (int t) {
   static int cache_age = 0;

   if (t > 0)  cache_age = t;
   return (cache_age);
}
