
/*** UNIQUE.   Return a unique number.
/
/    n = unique();
/   
/    Parameters:
/       int4   n;     (returned unique number)
/
/    Purpose:
/       Provide a source of always-unique numbers.
/
/    How it works:
/
/    Returns: continually increasing numbers
/
/    Known bugs:
/
/    Home:  sweb/unique.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/18/96 15:54 New function. */
/*: CR 11/05/04 Start from semi-random value. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

FUNCTION  int4 unique()
{
   static int4 next = L(0);

   if (! next)  next = time(NULL) % 10000;

   return (++next);
}
