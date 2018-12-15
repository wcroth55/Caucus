
/*** CHXSUBEQ.  Compare a substring against a chix for equality.
/
/    i = chxsubeq (x, pos, y);
/
/    Parameters:
/       int   i;   (1 if equal, 0 if not-equal)
/       Chix  x;   (source of substring)
/       int4  pos; (starting position of substring in X)
/       Chix  y;   (compare substring against Y)
/
/    Purpose:
/       Compare a substring of X against all of Y, for equality.
/       Two chix of different types are considered equal if their 
/       corresponding characters, after promotion (by adding leading 
/       0 bytes to the smaller sized characters) are identical.
/
/    How it works:
/       Call chxscan() to do the dogwork...
/
/    Returns: 1 if substring of X equals Y,
/             0 otherwise.
/             0 if either X or Y are not valid chix.
/
/    Error Conditions:
/  
/    Known bugs:      none
/
/    Home:  chx/chxsubeq.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/16/96 14:38 New function. */
/*: OQ 02/26/98 repalce call to chxcheck by in-line commands */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxsubeq (Chix x, int4 pos, Chix y)
{
   int4   x_end, y_end, differ_at;

   ENTRY ("chxsubeq", "");

   /*if (NOT chxcheck (x, "chxsubeq"))  RETURN (0);*/
   /*if (NOT chxcheck (y, "chxsubeq"))  RETURN (0);*/

   if ( x == nullchix || y == nullchix )
      { chxerror (CXE_NULL, "chxsubeq", "");          RETURN(0); }
   if (FREED(x))    
      { chxerror (CXE_FREED, "chxsubeq", x->name);    RETURN(0); }
   if (FREED(y))    
      { chxerror (CXE_FREED, "chxsubeq", y->name);    RETURN(0); }
   if (NOT ISCHIX(x) || NOT ISCHIX(y))
      { chxerror (CXE_NOTCHX, "chxsubeq", "");        RETURN(0); }
   
   differ_at = chxscan (x->p + pos * x->type, x->type, 
                        y->p,                 y->type, &x_end, &y_end);

   RETURN (differ_at == chxlen (y) ? 1 : 0);
}
