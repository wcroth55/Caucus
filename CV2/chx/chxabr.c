/*** CHXABR.  Check if one chix is an abbreviation of another.
/
/    i = chxabr (a, b);
/
/    Parameters:
/       int   i;
/       Chix  a, b;
/
/    Purpose:
/       Check if one chix is an abbreviation of another.
/       An empty Chix is not a valid abbreviation.
/
/    How it works:
/       Returns 1 if one of the chixen is an abbreviation of the other.
/
/    Returns: 1 if A is an abbreviation of B
/             2 if B is an abbreviation of A
/             0 if neither is an abbrevation of the other,
/             or if either is empty or not a Chix.
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
/    Home:  chx/chxabr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  6/04/92 15:36 New function. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: OQ 02/09/98 replace the call to chxcheck to in-line command */
#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxabr (a, b)
   Chix   a, b;
{
   ENTRY ("chxabr", "");

   if (a == nullchix || b == nullchix )
                     { chxerror (CXE_NULL, "chxabr" , "");        RETURN(0); }
   if (FREED(a))    
                     { chxerror (CXE_FREED, "chxabr" , a->name);   RETURN(0); }
   if (FREED(b))    
                     { chxerror (CXE_FREED, "chxabr" , b->name);   RETURN(0); }
   if ( (NOT ISCHIX(a)) || (NOT ISCHIX(b)) )
                     { chxerror (CXE_NOTCHX, "chxabr" , "");        RETURN(0); }
   if (EMPTYCHX (a) || EMPTYCHX (b)) RETURN (0);

   if (chxindex (a, 0L, b) == 0) RETURN (2);
   if (chxindex (b, 0L, a) == 0) RETURN (1);
   
   RETURN (0);
}

