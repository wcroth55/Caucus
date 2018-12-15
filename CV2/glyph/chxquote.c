/*** CHXQUOTE.  Remove/Replace quoted string in a Chix.
/
/   chxquote (a, what, keep);
/
/   Parameters:
/      Chix  a;       // Chix to look in.
/      int   what;    // 0 = Remove, 1 = Replace.
/      Chix  keep[];  // Array of Chix to store subchixen that were removed.
/
/   Purpose:
/    CHXQUOTE is used to temporarily remove a quoted string from
/    a command line, and then to put it back, later.  This means
/    that command parsers can ignore the effects of anything inside
/    a quoted string.
/
/   How it works:
/       chxquote (chx, 0, keep)    Removes a quoted string and keeps it in KEEP.
/       chxquote (chx, 1, keep)    Replaces the quoted string back into STR.
/
/   Returns:
/    CHXQUOTE (chx, 0, keep) returns 1 if a quoted string was found
/    and removed, and 0 otherwise.
/    CHXQUOTE (chx, 1, keep) returns 1 if the removed string was found
/    and replaced, and 0 otherwise.
/
/   Error Conditions:
/
/   Side effects:
/
/   Related Functions:
/
/   Called by:
/
/   Known bugs:
/
/   Home: glyph/mycrypt.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  5/19/92 13:13 Source code master starting point */
/*: CL 12/10/92 14:52 Long chxalter args. */

#include "caucus.h"
#include <stdio.h>

extern Chix ss_qt1, ss_qt2;

FUNCTION chxquote (chxa, what, keep)
   int   what;
   Chix  chxa, keep[];
{
   static Chix q1, q2;
   static char first=0;
   int    success;
   int4   p1, p2;

   ENTRY ("chxquote", "");

   if (NOT first) {
      first=1;
      q1 = chxalloc (L(6), THIN, "chxquote q1");
      chxofascii (q1, "\001\001\001");
      q2 = chxalloc (L(6), THIN, "chxquote q2");
      chxofascii (q2, "\002\002\002");
   }
   
   success = 0;

   if (what == 0) {
      p1 = chxinside (chxa, ss_qt1, keep[0], ss_qt1);
      if (p1 >= 0)  success += chxalter (chxa, p1, keep[0], q1);

      p2 = chxinside (chxa, ss_qt2, keep[1], ss_qt2);
      if (p2 >= 0)  success += chxalter (chxa, p2, keep[1], q2);
   }
   else {
      success += chxalter (chxa, L(0), q2, keep[1]);
      success += chxalter (chxa, L(0), q1, keep[0]);
   }

   RETURN (success);
}
