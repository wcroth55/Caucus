
/*** CHXALSUB.  Allocate a chix, put a substring in it.
/
/    c = chxalsub (a, i, n);
/
/    Parameters:
/       Chix  c;    (resulting string)
/       Chix  a;    (source string)
/       int4  i;    (starting position in A of substring)
/       int4  n;    (length of desired substring)
/
/    Purpose:
/       Allocate a new chix.  Put into it a substring of chix A, starting
/       at position I, N characters total.
/
/    How it works:
/       Uses existing chix functions.  Caller must remember to chxfree()
/       the return value from chxalsub().
/
/    Returns: a new chix.
/
/    Error Conditions:
/       If A is not a chix, chxalsub() returns immediately and complains
/       via chxerror().
/
/       Rediculous values of I or N are constrained to the beginning or end
/       of string A.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  chx/chxalsub.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/20/92 15:15 New function. */
/*: CR 12/10/92 17:17 Make I and N args long. */
/*: OQ 02/09/98 replace chxckech calls to  inline commands*/
#include <stdio.h>
#include "chixdef.h"

FUNCTION  Chix chxalsub (a, i, n)
   Chix   a;
   int4   i, n;
{
   Chix   result;

   ENTRY ("chxalsub", "");

   /*** Input validation.  (The validation of I and N is done inside
   /    chxcatsub(). ) */
   /*if (NOT chxcheck (a, "chxalsub"))   RETURN(nullchix);*/

   if (a == nullchix)
      { chxerror (CXE_NULL, "chxalsub", "");        RETURN(nullchix); }
   if (FREED(a))    
      { chxerror (CXE_FREED,"chxalsub", a->name);   RETURN(nullchix); }
   if (NOT ISCHIX(a))
      { chxerror (CXE_NOTCHX, "chxalsub", "");      RETURN(nullchix); }

   result = chxalloc (80L, a->type, "chxalsub result");
   chxcatsub (result, a, i, n);
   RETURN    (result);
}
