/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** DISPLAY_LICENSE.   Display the Caucus license information. */

/*: CR  3/29/99 17:30 New function, based on showlicense(). */
/*: CR  5/23/03 Add caucus_version() call */

#include <stdio.h>
#include "caucus.h"

FUNCTION  display_license()
{
   int4   expires;
   int    year, month, day, hour, minute, sec, wday;
   char  *strplain();
   char  *caucus_version();
   char  *months[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                      "JUL", "AUG", "SEP", "OCT", "NOV", "DEC", "XXX" };

   printf ("Caucus version:   %s\n",  caucus_version());

   return (1);
}
