
/*** CHXCLEAR.  Clear a chix, that is set it to the "empty" string.
/
/    chxclear (x);
/
/    Parameters:
/       Chix  x;
/
/    Purpose:
/       Clear a chix, that is set it to the "empty" string.
/
/    How it works:
/       Each byte in the first character is set to 0.
/
/    Returns: 1
/
/    Error Conditions:  none
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  Application
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  chx/chxclear.c.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: OQ 02/09/98 replace chxcheck call to in line commands */
/*: OQ 02/24/98 added code for single-byte character chix type */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxclear (x)
   Chix   x;
{
   int    i;

   ENTRY ("chxclear", "");

   /*if (NOT chxcheck (x, "chxclear"))  RETURN (1);*/
   if (x == nullchix)
      { chxerror (CXE_NULL, "chxclear", "");          RETURN(1); }
   if (FREED(x))    
      { chxerror (CXE_FREED, "chxclear", x->name);    RETURN(1); }
   if (NOT ISCHIX(x))
      { chxerror (CXE_NOTCHX, "chxclear", "");        RETURN(1); }
   
#if USE_THIN_CHIX
   x->p[0] = '\0';
#else
   for (i=0;   i<x->type;   ++i)  x->p[i] = '\0';
#endif
   x->actlen = 0L;

   RETURN  (1);
}
