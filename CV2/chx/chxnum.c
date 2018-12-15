
/*** CHXNUM.  Convert digits in a chix to a number.
/
/    success = chxnum (a, v);
/
/    Parameters:
/       int   success;   (found & converted a number?)
/       Chix  a;         (convert digits in this chix...)
/       int4 *v;         (and put them in this long)
/
/    Purpose:
/       Convert the digits found at the beginning of chix A to a long
/       integer.  Ignores leading blanks, and understands a leading
/       minus sign.
/
/    How it works:
/       Most of the work is done by calling chxlong().  If no number is
/       found, V is set to 0.
/
/    Returns: 1 if any digits were found (and converted)
/             0 if no  digits were found (or other error)
/
/    Error Conditions:
/       no digits were found
/       A is not a chix
/       V is NULL
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  chx/chxnum.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/09/91 16:21 New function. */
/*: CR 10/11/92 21:32 Expand comments. */
/*: CR 12/11/92 15:32 Long chxint4 arg. */
/*: OQ  2/10/98 replace chxcheck with in line codes */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxnum (a, v)
   Chix   a;
   int4  *v;
{
   int4   i;

   ENTRY ("chxnum", "");

   /*if (NOT chxcheck (a, "chxnum")  ||  v == (int4 *) NULL)  RETURN (0);*/
   if (a == nullchix)
      { chxerror (CXE_NULL, "chxnum", "");        RETURN(0); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxnum", a->name);  RETURN(0); }
   if (NOT ISCHIX(a))
      { chxerror (CXE_NOTCHX, "chxnum", "");      RETURN(0); }

   if ( v == (int4 *) NULL)  RETURN (0);
   i = 0;
   *v = chxint4 (a, &i);

   RETURN (i ? 1 : 0);
}
