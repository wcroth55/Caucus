
/*** CHG_FULL.   Actually change a user's name in the database.
/ (This should be changed to call chg_dperson(). */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/12/90 18:18 New function. */
/*: CR  7/02/91  0:00 Include "unitwipe.h". */
/*: CR  7/17/91 13:58 Call chgxperson() if distributed. */
/*: CR  7/28/91 17:14 Use addxperson(), not chgxperson(). */
/*: JV  7/29/91 12:59 Use chgxperson(), not addxperson(). */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CR  8/07/91 14:25 Add 'confnum' arg to syspdir() call. */
/*: CX 11/10/91 16:37 Chixify. */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: JX  5/19/92 12:07 Fix Chix. */
/*: CI 10/05/92 10:38 Chix Integration. */
/*: CR 10/11/92 22:46 Remove CQ from chgxperson() call. */
/*: CR  1/26/93 15:49 Use PLAIN and strplain() to decrypt license info. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */

#include <stdio.h>
#include "caucus.h"
#include "unitwipe.h"

extern struct pdir_t        pdir;
extern short                confnum;

FUNCTION  chg_full (oldname, oldlast, newname, newlast)
   Chix   oldname, oldlast, newname, newlast;
{
   Chix   userid, member;
   struct namlist_t  *add, *del, *nlnames(), *nlnode();
   short  c, i;
   int    error, success;

   ENTRY ("chg_full", "");

   userid = chxalloc (L(20), THIN, "chg_full userid");
   member = chxalloc (L(60), THIN, "chg_full member");
   sysuserid (userid);

   /*** Replace the user's old name with the new name.  Create a list of
   /    nameparts to DELete, and a list of nameparts to ADD.  Hand them
   /    off to MODNAMES to do the dirty work. */
   add = nlnames (newname, userid);
   del = nlnames (oldname, userid);
   c = modnames (XSND, XSNN, nullchix, 0, L(0), add, del, NOPUNCT);
   nlfree (add);
   nlfree (del);
   if (NOT c)  { success = 0;   goto done; }

   /*** If distributed, write change name transaction.  (What should be
   /    done if chgxperson() returns an error?) */
   /* 
      if (PLAIN(license.function_mask) & FM_DIST) {
         chgxperson (newname, oldname, userid, nullchix, &error);
      }
   */

   /*** Change this user's name in the membership list of every conference
   /    the user belongs to. (!)  */
   tomember (member, oldlast, oldname, userid);
   del = nlnode(1);   nladd (del, member, 0);
   tomember (member, newlast, newname, userid);
   add = nlnode(1);   nladd (add, member, 0);

   syspdir (&pdir, XUPA, 0, 0, userid);
   for (i=0;   i<pdir.parts;   ++i) {
      unit_wipe (LCNF);
      modnames (XCMD, XCMN, nullchix, pdir.resp[i], L(0), add, del, NOPUNCT);
   }
   nlfree (add);
   nlfree (del);
   success = 1;

done:
   chxfree (userid);
   chxfree (member);

   RETURN  (success);
}
