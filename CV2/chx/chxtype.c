
/*** CHXTYPE.  Return the type of a chix.
/
/    i = chxtype (x);
/
/    Parameters:
/       int   i;
/       Chix  x;
/
/    Purpose:
/       Return the type of a chix X.
/
/    How it works:
/
/    Returns: 1 if a THIN chix
/             2 if a WIDE chix
/             0 if not a chix
/
/    Error Conditions:  invalid chix (also calls chxerror to handle errors)
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  chx/chxtype.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CX  6/07/92 21:01 Null chix just returns 0 w/o complaining. */
/*: OQ  2/17/98 replace chxcheck by in line commands */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxtype (x)
   Chix   x;
{
   ENTRY ("chxtype", "");

   /*if (NOT chxcheck (x, "chxtype"))  RETURN (0);*/

   if (x == nullchix)                               RETURN (0);
   if (FREED(x))    
      { chxerror (CXE_FREED, "chxtype", x->name);   RETURN(0); }
   if (NOT ISCHIX(x))
      { chxerror (CXE_NOTCHX, "chxtype", "");       RETURN(0); }
   
   RETURN (x->type);
}

