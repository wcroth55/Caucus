
/*** CHXEQ.  Compare two chix for equality.
/
/    i = chxeq (x, y);
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
/       chix are 0.
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
/    Home:  chx/chxeq.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: OQ 02/09/98 replace chxcheck call by in line commands */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxeq (x, y)
   Chix   x, y;
{
   int4   x_end, y_end;

   ENTRY ("chxeq", "");

/* if (NOT chxcheck (x, "chxeq"))  RETURN (0);  */
/* if (NOT chxcheck (y, "chxeq"))  RETURN (0);  */

   if (x == nullchix || y == nullchix )
      { chxerror (CXE_NULL,  "chxeq" , "");        RETURN(0); }
   if (FREED(x))    
      { chxerror (CXE_FREED, "chxeq" , x->name);   RETURN(0); }
   if (FREED(y))    
      { chxerror (CXE_FREED, "chxeq" , y->name);   RETURN(0); }

   if ( (NOT ISCHIX(x)) || (NOT ISCHIX(y)) )
      { chxerror (CXE_NOTCHX, "chxeq" , "");       RETURN(0); }
   
#if USE_THIN_CHIX
   RETURN  (strcmp ((char *) x->p, (char *) y->p) == 0);

#else
   chxscan (x->p, x->type, y->p, y->type, &x_end, &y_end);

   RETURN  (x_end==0L  &&  y_end==0L);
#endif
}
