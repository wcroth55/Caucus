
/*** CHXCHECK.  Check a chix argument: is it really a chix?
/
/    i = chxcheck (x, name);
/
/    Parameters:
/       int   i;
/       Chix  x;
/       char *name;    (name of calling function)
/
/    Purpose:
/       Determine if argument X is really a chix, to the extent possible.
/
/    How it works:
/       To be declared a chix, X must pass three tests:
/          (1) it must not be a null pointer
/          (2) the "ischix" member must not be CHIX_FREED
/          (3) the "ischix" member must be CHIX_MAGIC
/
/       If X fails any test, chxcheck() calls chxerror() to complain
/       appropriately.
/
/    Returns: 1 on success, 0 if X fails any test.
/
/    Error Conditions:  none
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  Practically everybody.
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  chx/chxcheck.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: JX  6/08/92 19:41 Broke up lines for breakpointing. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxcheck (x, name)
   Chix   x;
   char  *name;
{
   ENTRY ("chxcheck", "");

   if (x == nullchix)
                     { chxerror (CXE_NULL,   name, "");        RETURN(0); }
   if (FREED(x))    
                     { chxerror (CXE_FREED,  name, x->name);   RETURN(0); }
   if (NOT ISCHIX(x))
                     { chxerror (CXE_NOTCHX, name, "");        RETURN(0); }
   
   RETURN  (1);
}
