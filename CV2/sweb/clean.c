
/*** CLEAN_NEWLINES.
/
/    clean_newlines (x);
/   
/    Parameters:
/       Chix         x;   (chix to be cleaned of newlines)
/
/    Purpose:
/       Replace all newlines in X with blanks.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  clean.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/26/95 13:05 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

FUNCTION  void clean_newlines (Chix x)
{
   static Chix  newline = (Chix) NULL;
   static Chix  blank   = (Chix) NULL;

   /*** Initialization. */
   if (newline == (Chix) NULL) {
      newline = chxalsub (CQ("\n"), 0, 4);
      blank   = chxalsub (CQ( " "), 0, 4);
   }

/* ---original code, calling chxreplace should be much faster. 
   while (chxalter (x, L(0), newline, blank)) ; */

   chxreplace (x, L(0), newline, blank);

   return;
}
