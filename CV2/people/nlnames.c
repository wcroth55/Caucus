/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** NLNAMES.  Create a list of names.
/
/    Given a string STR containing one or more nameparts separated by
/    blanks, NLNAMES creates and returns a sorted namelist, one namepart
/    per node.  Each namepart is assigned the owner OWNER. */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CR 12/20/88 17:45 Raise size of allowed strings. */
/*: JV  3/17/90 19:51 Removed stdio.h */
/*: DE  4/14/92 15:23 Chixified */
/*: JX  6/08/92 13:23 chxreduce->jixreduce. */
/*: CI 10/07/92 14:17 Check for null owner. */
/*: CR 12/17/92 12:20 Check chxtoken >=0 . */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  struct namlist_t *nlnames (str, owner)
   Chix  str, owner;
{
   struct namlist_t *p, *nlnode();
   Chix   name;
   short  i;

   ENTRY ("nlnames", "");

   name     = chxalloc (L(120), THIN, "nlnames name");

   p = nlnode(1);
   for (i=1;   chxtoken (name, null.chx, i, str) >= 0;   ++i) {
      jixreduce (name);
      if (owner!=nullchix  &&  NOT EMPTYCHX(owner)) {
         chxcat   (name, CQ(" "));
         chxcat   (name, owner);
      }
      nladd    (p, name, 0);
   }

   chxfree ( name );

   RETURN (p);
}
