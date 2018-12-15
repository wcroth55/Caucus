/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** BAD_NAME.  Is this is a BAD user name? 
/
/    BAD_NAME returns 1 if NAME is empty, contains any non-alphabetic
/    characters, or is more than 8 characters int4.  BAD_NAME converts
/    NAME to lower case in the process. */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CW  5/18/89 14:24 Add XW unitcodes. */
/*: DE  4/09/92 16:41 Chixified */
/*: JX  5/27/92 13:20 Fixify. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */

#include "caucus.h"
#include <stdio.h>

extern union null_t null;

FUNCTION  bad_name (name)
   Chix   name;
{
   int4 tempchar;
   int  s;

   ENTRY ("bad_name", "");

   if (EMPTYCHX (name))  RETURN (1);

   jixreduce (name);
   for (s=0;   s < chxlen (name);   ++s)
      tempchar = chxvalue (name, L(s));
      if (NOT cf_lower((int) tempchar)  &&  NOT cf_digit((int) tempchar)) {
         mdwrite (XWER, "gen_Tnamebad", null.md);
         RETURN  (1);
      }

   if (chxlen (name) > 7) {
      mdwrite (XWER, "gen_Tnameint4", null.md);
      RETURN  (1);
   }

   RETURN (0);
}
