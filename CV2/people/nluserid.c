
/*** NLUSERID.  Turn a person's name into a list of userids.
/
/    NLUSERID creates a name-list of the userids of all persons
/    whose names fit NAME.  If INCONF is true, the list only contains
/    members of the current conference. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: JV  3/17/90 19:51 Removed stdio.h */
/*: CR  3/29/91 15:47 Add AND/OR code to matchnames() call. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  7/17/91 12:38 Don't try to lock remote users. */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: CR 11/07/91 14:18 Add confnum arg to matchnames() call. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: DE  4/14/92 17:09 Chixified */
/*: CR  3/20/95 13:08 Check for <CANCEL> at appropriate times. */
/*: CR 10/12/01 16:40 Ignore @'s!  Replace with "^" for now. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;
extern short        confnum;

FUNCTION  struct namlist_t *nluserid (name, inconf)
   Chix   name;
   int    inconf;
{
   struct namlist_t *names, *matches, *p, *results, *nlnames(), *nlnode();
   struct namlist_t *success;
   Chix   temp, id, host, localhost;
   int    local, count;
   short  c;
   
   ENTRY ("nluserid", "");

   temp      = chxalloc (L(100), THIN, "nluserid temp");
   id        = chxalloc (L(80),  THIN, "nluserid id");
   host      = chxalloc (L(100), THIN, "nluserid host");
   localhost = chxalloc (L(100), THIN, "nluserid localhost");

   getmyhost (localhost);
   results = nlnode(1);
   names   = nlnames (name, null.chx);
   c = matchnames (XSND, XSNN, 0, null.chx, names, AND, &matches, NOPUNCT);
   nlfree (names);
   if (c == 0)  DONE(results);

   /*** If INCONF, remove anyone not a member of this conference. */
   for (p=matches->next, count=0;   p!=null.nl;   p = p->next, ++count) {
      chxtoken (temp, null.chx, 2, p->str);
      chxbreak (temp, id, host, CQ("^"));
      local = (EMPTYCHX(host)  ||  chxeq (host, localhost));

      if      (NOT local  ||  NOT inconf)  nladd (results, temp, 0);
      else if (unit_lock (XUPA, READ, L(confnum), L(0), L(0), L(0), id)) {
         if (unit_check (XUPA))  nladd (results, id, 0);
         unit_unlk (XUPA);
      }

      /*** Sanity limit for large lists and user hit <CANCEL>. */
      if (count > 100  &&  sysbrktest())  break;
   }

   nlfree (matches);
   DONE(results);

 done:

   chxfree ( temp );
   chxfree ( id );
   chxfree ( host );
   chxfree ( localhost );

   RETURN ( success );
}
