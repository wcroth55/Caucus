
/*** CHXSIMPLIFY.   Remove unneeded blanks from a chix.
/
/    chxsimplify (x);
/
/    Parameters:
/       Chix  x;   
/
/    Purpose:
/       Removes leading, trailing, and multiple blanks from chix X.
/
/    How it works:
/
/    Returns: 1 normally, 0 if X not a chix.
/   
/    Error Conditions:  X not a chix.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies: none
/
/    Known bugs:
/       This version only knows about the regular ascii blank character
/       " ".  It should probably use the chxspecial BLANK stuff.
/
/    Home:  chx/chxsimp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/10/91 17:50 New function. */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: OQ  2/20/98 replace chxcheck by in linecode */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxsimplify (x)
   Chix   x;
{
   Chix   blank2, blank1, empty;

   ENTRY ("chxsimplify", "");

   /*** Input validation. */
   /*if (NOT chxcheck (x, "chxsimplify"))               RETURN (0);*/
   if (x == nullchix)
      { chxerror (CXE_NULL, "chxsimplify", "");         RETURN(0); }
   if (FREED(x))    
      { chxerror (CXE_FREED, "chxsimplify", x->name);   RETURN(0); }
   if (NOT ISCHIX(x))
      { chxerror (CXE_NOTCHX, "chxsimplify", "");       RETURN(0); }

   blank2 = chxquick ("  ", 2);
   blank1 = chxquick (" ",  3);
   empty  = chxquick ("",   4);
   while (chxalter (x, 0L, blank2, blank1)) ;
   if (chxvalue (x, 0L) == ' ')  chxalter (x, 0L, blank1, empty);
   chxtrim (x);

   RETURN (1);
}
