/*** SHOW_NEW.  Tell the user about new items, unseen items, new responses,
/               new participants, and new messages.
/
/    show_new (out, thisconf)
/
/  Parameters:
/      int    out;
/      struct partic_t *thisconf;
/
/  Purpose:
/     Lists the objects that the user hasn't seen yet.
/
/  How it works:
/
/  Returns:  1.
/
/  Error Conditions:
/
/  Side Effects:
/
/  Related to:
/
/  Called by:  status()
/
/  Home:   conf/shownew.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: JX  3/23/92 12:33 Chixify, add header. */
/*: DE  5/26/92 12:42 Fix missing & inconsistant args */
/*: CN  5/15/93 15:20 Remove items_flagged. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"

extern struct master_template master;
extern union  null_t          null;
extern short                  selected[];

FUNCTION  show_new (out, thisconf)
   int    out;
   struct partic_t *thisconf;
{
   short  i, no_header, new, i0, i1, found;
   Chix   str, message, format;

   ENTRY ("shownew", "");

   str       = chxalloc (L(80),  THIN, "shownew str");
   format    = chxalloc (L(80),  THIN, "shownew format");
   message   = chxalloc (L(200), THIN, "shownew message");

   /*** New items?  Find undeleted high (i1) and low (i0) item numbers. */
   for (i0=thisconf->items+1;   DELETED(i0)  &&  i0<=master.items;   ++i0) ;
   for (i1=master.items;        DELETED(i1)  &&  i1>=i0;             --i1) ;
   if (i0 <= i1) {
      mdstr (format, "new_Titems", null.md);
      if (i0 == i1)   chxformat (message, CQ("%s %d   \n"), L(i1), L(0),
                                                        format, nullchix);
      else            chxformat (message, CQ("%s %d-%d\n"), L(i0), L(i1),
                                                        format, nullchix);
      unit_write (out, message);
   }

   /*** List UNSEEN items, as well. */
   found = 0;
   for (i=1;   i<=thisconf->items;   ++i) {
      selected[i] = -1;
      if (NOT DELETED(i)  &&  thisconf->responses[i] == -1)
         selected[i] = found = 1;
   }
   if (found) {
      mdwrite (out, "new_Tunseen", null.md);
      if (NOT printrange (out, selected, thisconf->items, 0))
         mdwrite (out, "new_Tnoitems", null.md);
   }

   /*** New responses? */
   no_header = 1;
   for (i=1, new=0;   i<=thisconf->items;   ++i) {
      if (thisconf->responses[i] >= 0  &&
          thisconf->responses[i] <  master.responses[i]) {
         if (no_header)  mdwrite (out, "new_Trespon", null.md);
         no_header = 0;
         ++new;
         chxclear  (str);
         chxformat (str, chxquick("%5d%s",0), L(i), L(0),
                   ((new%10)==0 ? chxquick("\n",1) : nullchix), nullchix);
         unit_write (out, str);
      }
   }
   if (no_header)   mdwrite (out, "new_Tnoresp", null.md);
   unit_write (out, CQ("\n"));

   /*** New messages? */
   newmess (out);
   RETURN (1);
}
