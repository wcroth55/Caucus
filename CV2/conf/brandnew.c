/*** BRANDNEW.  Tell the user about brand new items, responses, or messages.
/
/  brandnew (thisconf)
/
/  Parameters:
/     struct partic_t *thisconf;
/
/  Purpose:
/     This dipslays part of the output of a STATUS command.
/    Something is brand new if it has appeared since the last chance the
/    user had to know it existed.  For this purpose we maintain the
/    oldmaster / master item index pair, and the messdir.oldnum /
/    messdir.newnum highest message number pair.
/
/  How it works:
/    In BRANDNEW, we load the up-to-the-minute info about items and messages,
/    and compare against the "old" info and against the "what the user has
/    seen" info (the USER structure).  If anything IS brand new, we tell
/    the user about it.
/
/  Returns:  1.
/
/  Error Conditions:
/
/  Side Effects:
/
/  Related to:
/
/  Called by: 
/
/  Home: conf/brandnew.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: CR  3/20/90 20:27 Use message_t mess. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  6/25/91 15:27 Add 'confnum' arg to srvldmas() calls. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JX  3/04/92 11:48 Chixify, add header. */
/*: DE  5/26/92 13:56 Fix chixification */
/*: CN  5/15/93 15:20 Remove items_flagged. */
/*: CK  9/14/93 22:39 Test return from unit_lock(), unit_view(). */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"

extern struct master_template master, oldmaster;
extern struct message_t       mess;
extern union  null_t          null;
extern Userreg                thisuser;
extern short                  confnum;
extern short                  selected[];

FUNCTION  brandnew (thisconf)
   struct partic_t *thisconf;
{
   short  i, found, i0, i1, highmess;
   Chix   str;

   ENTRY ("brandnew", "");

   str    = chxalloc (L(80), THIN, "str");

   /*** Look at the number of the highest message this user has, and see
   /    if it means we have some brand new messages. */
   highmess = 0;
   if (unit_lock  (XUMD, READ, L(0), L(0), L(0), L(0), thisuser->id)) {
      if (unit_view (XUMD)) {
         unit_short (XUMD, &highmess, 1);
         unit_close (XUMD);
      }
      unit_unlk  (XUMD);
   }

   if (highmess > mess.high_seen) {
      mdwrite (XWAN, "gen_Tnewmes", null.md);
      if (highmess > mess.high_seen+1) 
                  chxformat (str, CQ("%d-%d\n"), L(mess.high_seen+1),
                             L(highmess), null.chx, null.chx);
      else        chxformat (str, CQ("%d\n"), L(highmess), L(0), null.chx, 
                             null.chx);
      unit_write (XWAN, str);
      mess.high_seen = highmess;
   }

   /*** Load the master index, so we can check for brand new items,
   /    or brand new responses. */
   srvldmas (1, &master, confnum);

   /*** Tell user about any brand new items. */
   if (master.items > oldmaster.items) {
      for (i0=thisconf->items+1;   DELETED(i0)  &&  i0<=master.items;  
          ++i0) ;
      for (i1=master.items;        DELETED(i1)  &&  i1>=i0;            
          --i1) ;
      if (i0 <= i1) {
         mdwrite (XWAN, "fnd_Tnewitem", null.md);
         if (i0 == i1)
              chxformat (str, CQ("%d\n"),    L(i1), L(0), nullchix, nullchix);
         else chxformat (str, CQ("%d-%d\n"), L(i0), L(i1), nullchix, nullchix);
         unit_write (XWAN, str);
      }
   }

   /*** Tell user about brand new responses. */
   found = 0;
   for (i=1;   i<=thisconf->items;   ++i) {
      if (master.responses[i]    >  oldmaster.responses[i]    &&
          master.responses[i]    >  thisconf->responses[i]    &&
          thisconf->responses[i] >= oldmaster.responses[i]    &&
          thisconf->responses[i] > 0)  selected[i] = found = 1;

      else selected[i] = -1;
   }

   if (found) {
      mdwrite    (XWAN, "gen_Tnewresp", null.md);
      printrange (XWAN, selected, thisconf->items, 0);
   }

   /*** Synchronize old and new master indices. */
   touch_master();

   chxfree (str);
   RETURN (1);
}
