/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CHXUPPER.  Convert Chix X so that all ASCII characters are
/    upper-case. */

/*: JX  5/28/92 14:35 New function (from strupper()). */
/*: CR 12/11/92 13:32 Increase speed; make chxsetval args int4. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"

extern struct flag_template flag;

FUNCTION  chxupper (x)
   Chix  x;
{
   int4  i;
   int4  charval;

   ENTRY ("chxupper", "");

   for (i=0;   (1);   ++i) {
      if ( (charval = chxvalue (x, L(i))) == 0)  break;
      if (charval >= 'a'  &&  charval <= 'z')
         chxsetval (x, i, THIN, charval + 'A' - 'a');
   }

   RETURN (1);
}
