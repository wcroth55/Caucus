/*** CHXSTRIP.  Strip quotes off of a quoted string.
/
/   chxstrip (a)
/
/   Parameter:  Chix a;  // Chix to look in.
/
/   Purpose:
/    STRSTRIP removes quotes from a quoted string STR, and throws
/    away any text that follows the quoted string.  It leaves unquoted
/    strings alone. 
/
/   Returns: 1.
/
/   Error Conditions:
/
/   Side effects:
/
/   Related Functions: chxquote()
/
/   Called by: chg/chgsubject()
/
/   Known bugs:
/
/   Home: glyph/chxstrip.c */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  5/19/92 14:56 Create Function. */
/*: JX  9/30/92 16:29 One CQ per line. */
/*: CL 12/10/92 14:52 Long chxalter args. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "caucus.h"

extern Chix ss_qt1, ss_qt2;

FUNCTION  chxstrip (str)
   Chix   str;
{
   Chix   keep[2], temp;

   ENTRY ("chxstrip", "");

   temp    = chxalloc (L(100), THIN, "chxstrip temp");
   keep[0] = chxalloc (L(100), THIN, "chxstrip keep0");
   keep[1] = chxalloc (L(100), THIN, "chxstrip keep1");

   if (chxquote (str, 0, keep)) {
      while  (chxalter (str, L(0), CQ(" "),  chxquick("", 1))) ;
      while  (chxalter (str, L(0), ss_qt1,   CQ(" "))) ;
      while  (chxalter (str, L(0), ss_qt2,   CQ(" "))) ;
      chxcpy (temp, str);
      chxtoken (str, nullchix, 1, temp);
      chxquote (str, 1, keep);
      chxsimplify (str);
   }

   chxfree (temp);
   chxfree (keep[0]);
   chxfree (keep[1]);

   RETURN (1);
}
