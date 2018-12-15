
/*** CHXBLANK.  Is a char a "blank"?
/
/    yes = chxblank (cval);
/
/    Parameters:
/       int   yes;    (1=yes, 0=no)
/       int4  cval;   (value of character to be tested)
/
/    Purpose:
/       Determine if a particular character value is (logically speaking)
/       a "blank".  The list of such characters is set in chxinit(), but
/       it usually includes the regular space character, the tab, a newline,
/       and the Kanji double-wide space character.
/
/    How it works:
/       This function should be called by infrequently used functions
/       for simplicity.  For CPU intensive functions, the relevant
/       code should be extracted from this function and placed in-line
/       in the original function.
/
/    Returns: 1 if "blank", 0 otherwise.
/
/    Home:  chx/chxblank.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/25/98 15:11 New function. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxblank (int4 cval)
{
   static char  blanktab[256];
   static int4 *blanklist = NULL;
   Chix         blanks;
   int          sp, j;
   char        *sysmem();

   ENTRY ("chxblank", "");

   /*** Initialization: Get the list of "blank" characters.  If it's 
   /    empty, use the standard ascii blank, ' '. */
   if (blanklist == NULL) {
      blanks = chxalloc (16L, THIN, "chxtoken blanks");
      chxspecial (SC_GET, SC_BLANK, blanks);
      if (chxvalue (blanks, 0L) == 0L)  chxcatval (blanks, THIN, (int4) ' ');
      blanklist = (int4 *) sysmem (L(sizeof(int4) * (chxlen (blanks) + 2)),"blanklist");
      for (j=0;   (blanklist[j] = chxvalue (blanks, L(j)));   ++j)  ;
      for (j=0;   j < 256;        ++j)  blanktab[j] = 0;
      for (j=0;   blanklist[j];   ++j)  {
         if (blanklist[j] < 256)  blanktab[blanklist[j]] = 1;
      }
      chxfree (blanks);
   }


#if USE_THIN_CHIX
   RETURN (blanktab [cval]);
#else
   if (cval < 256)  RETURN (blanktab[cval]);

   for (sp=0;   blanklist[sp];   ++sp)
      if (cval == blanklist[sp])  return (1);

   RETURN (0);
#endif
}
