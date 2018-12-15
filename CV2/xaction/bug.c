/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** BUG.  Prints out a bug message somewhere.
*/

/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  8/04/91 08:04 Add buglist. */
/*: JV  8/13/91 14:01 Remove re-decl of sprintf cuz 3B won't compile */
/*: CR  9/18/91 10:29 Call buglist() with STR, not return from sprintf(). */

#include <stdio.h>

extern int debug;

bug (text, num)
   char *text; int num;
{
   char str[160];

   if (debug) 
      printf ("%s %d\n", text, num);
   else {
      sprintf (str, "%s %d\n", text, num);
      buglist (str);
   }
}
