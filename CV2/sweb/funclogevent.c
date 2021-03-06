
/*** FUNC_LOGEVENT.   $logevent()
/
/    func_logevent (arg);
/   
/    Parameters:
/       Chix             arg;     (function arguments)
/
/    Purpose:
/       Implement $logevent (event [iface duration size conf item r0 rn object 
/                            s1 s2 s3 n1 n2 n3])
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/
/    Notes: can we make this a delayed insert?
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/23/03 New function. */
/*: CR 10/20/04 Do not force logging if not already on, even if query fails. */

#include <stdio.h>
#include <time.h>
#include "sqlcaucus.h"
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define SIZE_EVENT   12   /* should probably pull these out of database */
#define SIZE_USERID  80
#define SIZE_SESSION 22

static Chix word = nullchix;
static char userid[SIZE_USERID+2];
static char sessionid[SIZE_SESSION];
static int  time0 = 0;
static int  loggedout = 0;
static int  timehere[2] = {0, 0};

FUNCTION  func_logevent (Chix arg) {
   char   stmt[6000], value[4000];
   char   event[SIZE_EVENT+2];
   int    year, month, day, hour, minute, sec, wday, rc, sql_lice;
   int    t0[2], t1[2], tdel[2];
   int    is_logout = 0;
   char   temp[100];
   char  *strplain();
   int    event_field (char *field, char type, int wnum, Chix arg,
                       char *stmt,  char *value, Chix word);
   ENTRY ("func_logevent", "");

   if (word == nullchix) {
      word    = chxalloc (L(40), THIN, "func_logevent word");
      sysuserid (word);
      ascofchx  (userid, word, 0, SIZE_USERID+1);
      sprintf   (sessionid, "%010d%08d", time(NULL), (int) getpid());

      time0 = time(NULL);
   }

   /*** Get event type.  'Logout' is special -- allow only one.  It may
   /    come from a CML script $logevent() call, or it may be automatically
   /    generated by a timeout or kill.  In the latter case, calculate
   /    duration from 'time0'. */
   if (chxtoken (word, nullchix, 1, arg) < 0)  RETURN (0);
   is_logout = chxeq (word, chxquick("logout", 0));
   if (is_logout) {
      if (loggedout)  RETURN (0);
      loggedout = 1;
      if (chxtoken (word, nullchix, 3, arg) < 0) {
         sprintf (temp, "logout _ %d", time(NULL) - time0);
         chxofascii (arg, temp);
      }
      chxofascii (word, "logout");
   }

   /*** event, userid, sessionid, time */
   strcpy    (stmt, "insert into events (event, userid, sessionid, time ");
   ascofchx  (event, word, 0, SIZE_EVENT+1);
   strlower  (event);
   sysymd    (time(NULL), &year, &month, &day, &hour, &minute, &sec, &wday);
   sprintf   (value, "'%s', '%s', '%s', '%04d-%02d-%02d %02d:%02d:%02d'",
              event, userid, sessionid, year, month, day, hour, minute,
              sec);

   event_field ("iface",    'S',  2, arg, stmt, value, word);
   event_field ("duration", 'I',  3, arg, stmt, value, word);
   event_field ("size",     'I',  4, arg, stmt, value, word);
   event_field ("conf",     'I',  5, arg, stmt, value, word);
   event_field ("item",     'I',  6, arg, stmt, value, word);
   event_field ("r0",       'I',  7, arg, stmt, value, word);
   event_field ("rn",       'I',  8, arg, stmt, value, word);
   event_field ("object",   'S',  9, arg, stmt, value, word);
   event_field ("s1",       'S', 10, arg, stmt, value, word);
   event_field ("s2",       'S', 11, arg, stmt, value, word);
   event_field ("s3",       'S', 12, arg, stmt, value, word);
   if (is_logout) {
      event_field ("n1",    'V', timehere[0], arg, stmt, value, word);
      event_field ("n2",    'V', timehere[1], arg, stmt, value, word);
   }
   else {
      event_field ("n1",    'I', 13, arg, stmt, value, word);
      event_field ("n2",    'I', 14, arg, stmt, value, word);
      event_field ("n3",    'I', 15, arg, stmt, value, word);
   }

   strcat  (stmt, ") values (" );
   strcat  (stmt, value);
   strcat  (stmt, ")");

   bigtime (t0);
   rc = sql_exec (0, stmt);
   bigtime (t1);
   bigtime_sub (tdel, t1, t0);
   bigtime_add (timehere, timehere, tdel);

   if (rc != SQL_SUCCESS) {
      logger  (0, LOG_FILE, stmt); 
   }

   RETURN (0);
}

FUNCTION int event_field (char *field, char type, int wnum, Chix arg,
                          char *stmt,  char *value, Chix word) {
   int   num;
   char  temp[100];

   if (type != 'V'  &&  chxtoken (word, nullchix, wnum, arg) < 0) 
                                                  return (0);
   if (type == 'I') {
      if (NOT chxnum (word, &num))                return (0);
      sprintf (temp, ", %d", num);
   }
   else if (type == 'S') sprintf (temp, ", '%s'", ascquick(word));
   else if (type == 'V') sprintf (temp, ", %d", wnum);
   else                                           return (0);

   strcat  (value, temp);
   strcat  (stmt, ", ");
   strcat  (stmt, field);

   return (1);
}
