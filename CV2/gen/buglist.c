/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** BUGLIST.   Report a possible bug.
/
/    BUGLIST appends an entry to the "buglist" file, stamped with
/    date/time, userid, and name of conference. */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CR  8/29/89 16:18 Save copy of "bug" as it may be written over. */
/*: CR 10/24/89 13:00 Use sysuserid() to display real userid. */
/*: CR  3/29/90 22:11 Prevent recursion; output to terminal if no XSBG. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: JV 10/31/91 15:12 Declare sprintf */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: DE  4/07/92 13:27 Chixified */
/*: CX  5/21/92 17:14 Modify syssecure() use. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: CR 10/12/05 Remove mdwrite() call. */

#include <stdio.h>
#include "caucus.h"

extern Userreg              thisuser;
extern Chix                 confname;
extern short                confnum;
extern union  null_t        null;

FUNCTION  buglist (bug)
   char   bug[];
{
   static char in_buglist = 0;
   Chix   date, time, tempchx;
   Chix   uid, str, thebug;
   int    ok;
   int4   systime();
   char  *tempbug, *sysmem();

   /*** Recursive calls to buglist() return immediately! */
   if (in_buglist)   return;
   in_buglist = 1;

   date     = chxalloc (L(40),  THIN, "buglist date");
   time     = chxalloc (L(40),  THIN, "buglist time");
   uid      = chxalloc (L(24),  THIN, "buglist uid");
   str      = chxalloc (L(100), THIN, "buglist str");
   thebug   = chxalloc (L(200), THIN, "buglist thebug");

   /*** Remove any security information from BUG and convert it to
   /    chix THEBUG. */
   tempbug = sysmem (L(strlen(bug))+10, "tempbug");
   if (tempbug != NULL) {
      strcpy    (tempbug, bug);
      syssecure (tempbug);
      chxcpy    (thebug, CQ(tempbug));
      sysfree   (tempbug);
   }
   else chxcpy (thebug, CQ(bug));

   sysdaytime (date, time, 0, systime());
   if (chxvalue(date,0L)==' ') {
      tempchx = CHXSUB(date, L(1));
      chxcpy  (date,CQ("0"));
      chxcat  (date,tempchx);
      chxfree (tempchx);
    }
   sysuserid (uid);
   chxclear  (str);
   chxformat (str, CQ("\n%s %s"),    L(0), L(0), date, time);
   chxformat (str, CQ(" %s %s"),     L(0), L(0), uid,  thisuser->id);
   chxformat (str, CQ(" %s %d\n  "), L(confnum), L(0),  confname, null.chx);

   if (ok = unit_lock (XSBG, WRITE, L(0), L(0), L(0), L(0), null.chx)) {
      if (ok = unit_append (XSBG)) {
         unit_write (XSBG, str);
         unit_write (XSBG, thebug);
         unit_close (XSBG);
      }
      unit_unlk (XSBG);
   }

   in_buglist = 0;

   chxfree ( date );
   chxfree ( time );
   chxfree ( uid );
   chxfree ( str );
   chxfree ( thebug );

}
