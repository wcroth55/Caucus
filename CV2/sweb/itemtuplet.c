
/*** ITEM_TUPLET.  Convert 6-tuple to "dotted" item number.
/
/    item_tuplet (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $item_tuplet(i0 i1 i2 i3 i4 i5)
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Home:  itemtuplet.c
/ */

/*** Copyright (C) 1995-2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 04/02/06 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

#define TEMP(n)   chxtoken (temp, nullchix, n, arg)

extern int       cml_debug;

FUNCTION  item_tuplet (Chix result, Chix arg) {
   static Chix   temp = nullchix;
   int           i;
   ENTRY ("item_tuplet", "");

   /*** Initialization. */
   if (temp == nullchix) {
      temp  = chxalloc     (L(40), THIN, "item_tup temp");
   }

   for (i=1;   i<=6;   ++i) {
      if (chxtoken (temp, nullchix, i, arg) < 0)  return(1);
      if (chxvalue (temp, 0L) == '0')             return(1);
      if (i > 1)  chxcatval (result, THIN, L('.'));
      chxcat (result, temp);
   }
   return (1);
}
