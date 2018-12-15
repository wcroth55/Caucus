/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** AMBIGUOUS.  Complain to the user about an ambiguous word.  */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CW  5/18/89 14:24 Add XW unitcodes. */
/*: CX 10/20/91 18:12 Chixify. */
/*: CL 12/10/92 14:52 Long chxalter args. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  ambiguous (word)
   Chix   word;
{
   static Chix  s_ambig;
   Chix         str;

   ENTRY ("ambiguous", "");

   str = chxalloc (L(80), THIN, "ambiguous str");
   chxcpy (str, word);
   while  (chxalter (str, L(0), chxquick (" ", 1), CQ("")) ) ;

   mdwrite    (XWER, "gen_Fambig1", null.md);
   unit_write (XWER, str);
   mdwrite    (XWER, "gen_Fambig2", &s_ambig);
   chxfree    (str);

   RETURN (1);
}
