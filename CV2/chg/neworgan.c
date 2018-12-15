
/*** Neworganizer.     Change the organizer. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:34 Source code master starting point */
/*: CR  4/28/89 13:06 Check return from unit_lock (XCMA... */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CR  7/13/90 14:33 Only the primary organizer can do this. */
/*: CR  2/01/91 12:49 Use person_format() to display user's name. */
/*: CR  3/29/91 16:37 Use AND/OR in matchnames() call. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  6/25/91 15:27 Add 'confnum' arg to srvldmas() calls. */
/*: CR  7/16/91 17:08 Add HOST argument to person_format() calls. */
/*: CR  7/30/91 18:11 Add 3rd arg to loadreg(). */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: CR 11/05/91 23:10 Use cus_dporg(). */
/*: CR 11/06/91 16:21 Add cusxporg() call. */
/*: CR 11/07/91 14:18 Add confnum arg to matchnames() call. */
/*: UL 12/11/91 18:41 Make unit_lock args 3-6 int4.*/
/*: DE  3/09/92 14:06 Chixified */
/*: DE  4/10/92 14:06 Converted sysfree -> chxfree */
/*: JX  5/19/92 20:58 Fix Chix. */
/*: JX  7/07/92 10:14 Allocate ID earlier, dealloc FORMAT. */
/*: JX  7/07/92 19:56 Only call cusxporg() if conf is distributed. */
/*: JX  8/03/92 10:13 CHN's are chix. */
/*: CR 10/11/92 21:53 Remove CQ from (md)prompter() call. */

#include <stdio.h>
#include "caucus.h"

extern struct master_template  master;
extern union  null_t           null;
extern Chix   yesnotable;
extern Chix   ss_dotnl;
extern short  confnum;

FUNCTION  neworganizer()
{
   struct namlist_t *names, *owners, *nlnames();
   Userreg  neworg, make_userreg();
   int    error, success;
   short  number, c, is_member, ok;
   Chix   newname, answer, id, host;
   Chix   def, result, format, person_format();

   ENTRY ("neworgan", "");

   id = chxalloc (L(20), THIN, "neworgan id");

   /*** Check to ensure that only the primary organizer can specify
   /    a new primary organizer. */
   sysuserid (id);
   if (NOT equal_id (id, master.organizerid, nullchix)) {
      mdwrite (XWER, "cus_Tonlypri", null.md);
      chxfree (id);
      RETURN (0);
   }

   newname   = chxalloc (L(82), THIN, "neworgan newname");
   answer    = chxalloc (L(20), THIN, "neworgan answer");
   format    = chxalloc (L(80), THIN, "neworgan format");
   host      = chxalloc (L(80), THIN, "neworgan host"); 
   def       = chxalloc (L(20), THIN, "neworgan def");
   neworg    = make_userreg();
   success = 0;

   /*** Loop until the Organizer specifies a new Organizer, or gives up. */
   while (1) {
      if (mdprompter ("cus_Pneworg", nullchix, 0, "cus_Hneworg", 80, 
                       newname, nullchix) < -1)  FAIL;

      /*** Find the list of OWNERS userids that could be this person. */
      names = nlnames (newname, nullchix);
      c = matchnames (XSND, XSNN, 0, nullchix, names, AND, &owners, NOPUNCT);
      nlfree (names);
      if (c != 1)  FAIL;
      number = nlsize (owners);

      /*** If there's only one match, find out who it is. */
      if (number == 1) {
         neworg->briefs = neworg->print = null.nl;
         chxtoken (id, nullchix, 2, (owners->next)->str);
         loadreg (id, neworg, NULLSITE);

         /*** Is this person a member of the conference? */
         is_member = 0;
         if (unit_lock (XUPA, READ, L(confnum), L(0), L(0), L(0), neworg->id)) {
            is_member = unit_check (XUPA);
            unit_unlk (XUPA);
         }
      }

      /*** If NAME only matched one person, get the Organizer to confirm
      /    the change before going ahead and doing it.   If OK'd, rewrite
      /    the MASTER file with the new organizer's userid. */
      if (number == 1  &&  is_member) {
         mdstr    (format, "cus_Fokneworg", null.md);
         result = person_format (format, neworg->name,  neworg->id, nullchix,
                                         neworg->phone, null.nl);
         chxtoken (def, nullchix, 1, yesnotable);
         ok = prompter (result, yesnotable, 2, "cus_Hokneworg", 10, 
                                answer, def);
         chxfree (result);
         if (ok > 0) {
            getmyhost (host);
            if (cus_dporg (confnum, neworg->id, &master, &error) &&
                is_distributed (confnum))
               cusxporg  (confnum, neworg->id, host, nullchix, &error);
            break;
         }
      }

      else if (number == 1  &&  NOT is_member) {
         mdstr    (format, "cus_Fnotmem", null.md);
         result = person_format (format, neworg->name,  neworg->id, nullchix,
                                         neworg->phone, null.nl);
         unit_write (XWTX, result);
         chxfree    (result);
      }

      else if (number == 0) {
         mdwrite    (XWTX, "cus_Tnopart", null.md);
         unit_write (XWTX, newname);
         unit_write (XWTX, ss_dotnl);
      }

      else  toomany (newname, owners);
   }

   nlfree (owners);
   SUCCEED;

done:
   chxfree (id);
   chxfree (host);
   chxfree (def);
   chxfree (answer);
   chxfree (format);
   chxfree (newname);
   free_userreg (neworg);

   RETURN  (success);
}
