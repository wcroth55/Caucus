
/*** CHXEQF.  Compare two chix for equality. (Fast version)
/
/    i = chxeqf (x, y);
/
/    Parameters:
/       int   i;   (1 if equal, 0 if not-equal)
/       Chix  x;
/       Chix  y;
/
/    Purpose:
/       Compare two chix for equality.  Two chix of different types
/       are considered equal if their corresponding characters, after
/       promotion (by adding leading 0 bytes to the smaller sized
/       characters) are identical.
/
/    How it works:
/       Call chxscan() to do the dogwork, then see if the end of both
/       chix are 0.  DOES NOT DO ANY ERROR CHECKING OF ARGS!
/       This function should only be used in place of chxeq()
/       when the caller is absolutely certain that the args are valid.
/
/    Returns: 1 if X and Y are equal, 0 otherwise.
/       Also returns 0 if either X or Y are not valid chix.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  application
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Speed: macro instead of call?
/
/    Home:  chx/chxeqf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 06/16/98 New function, based on chxeq(). */
/*: CR 06/23/98 speed: replace chxscan() with strcmp. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxeqf (x, y)
   Chix   x, y;
{
   int4   x_end, y_end;

   ENTRY ("chxeqf", "");

#if USE_THIN_CHIX
   RETURN  (strcmp ( (char *) x->p, (char *) y->p) == 0);

#else
   chxscan (x->p, x->type, y->p, y->type, &x_end, &y_end);

   RETURN  (x_end==0L  &&  y_end==0L);
#endif

}
