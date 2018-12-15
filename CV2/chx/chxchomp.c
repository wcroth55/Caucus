
/*** CHXCHOMP.   Trim trailing newlines or carriage returns from a chix.
/
/    chxchomp (x);
/
/    Parameters:
/       Chix  x;   
/
/    Purpose:
/       Remove any newlines (012) or returns (015) at the end of chix X.
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
/    Home:  chx/chxchomp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/17/98 11:50 New function. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxchomp (x)
   Chix   x;
{
   int4   v;
   int    i, last;
   uchar *p;

   ENTRY ("chxchomp", "");

   /*** Input validation. */
   if (x == nullchix)
      { chxerror (CXE_NULL,   "chxchomp", "");        RETURN(0); }
   if (FREED(x))    
      { chxerror (CXE_FREED,  "chxchomp", x->name);   RETURN(0); }
   if (NOT ISCHIX(x))
      { chxerror (CXE_NOTCHX, "chxchomp", "");        RETURN(0); }

   /*** Find the position of the LAST character. */
   last = (x->actlen >= 0 ? x->actlen : chxlen (x)) - 1;

   /*** While that character is a blank, back up one. */
#if USE_THIN_CHIX
   for (p=x->p;   last >= 0;   --last) {
      if (p[last] != '\n'  &&  p[last] != '\r')  break;
   }
   p[++last] = '\0';

#else
   for (;   last >= 0;   --last) {
      p = x->p + x->type * last;
      v = *p++ & 0xFF;
      for (i=1;   i<x->type;   ++i)  { v = v << 8;   v += (*p++ & 0xFF); }
      if  (v != '\n'  &&  v != '\r')  break;
   }

   ++last;
   p = x->p + x->type * last;
   for (i=0;   i<x->type;   ++i)  *p++ = 0;
#endif
   x->actlen = last;

   RETURN (1);
}
