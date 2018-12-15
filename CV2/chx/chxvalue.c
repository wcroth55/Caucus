
/*** CHXVALUE.  Return value of a selected character in a chix.
/
/    v = chxvalue (x, n);
/
/    Parameters:
/       int4  v;   (value of N'th character of X)
/       Chix  x;   
/       int4  n;
/
/    Purpose:
/       Return the value of the N'th character of chix X as a int4 int.
/       The first character of a chix is character number 0.
/
/       Frequently used to determine if a chix is "empty", i.e. if
/       the first character contains all 0 bytes.
/
/    How it works:
/
/    Returns: value of N'th character, or -1 on error.
/   
/    Error Conditions:  X not a chix.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  application and some chx...() functions.
/
/    Operating system dependencies: none
/
/    Known bugs:
/       Limitation: requires that a character in a chix be no more than 
/       sizeof(int4) bytes, typically 4.
/
/    Speed: perhaps a fast equivalent, where x is guaranteed to be
/       a chix?  Any other way to reduce?  A macro instead of a
/       function? a chxvaluef function w/o arg checking?
/
/    Home:  chx/chxvalue.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR 12/11/92 13:33 Make last arg long. */
/*: CR 12/18/92 13:59 Speed up function by testing x->actlen. */
/*: OQ 11/19/97 11:30 speed up function by changing chxcheck to online code */
/*: OQ 11/19/97 11:30 speed up function by adding separate code for 1 byt ch*/

#include <stdio.h>
#include "chixdef.h"

FUNCTION  int4 chxvalue (x, n)
   Chix   x;
   int4   n;
{
   int4   v;
   int    i;
   uchar *p;

   ENTRY ("chxvalue", "");

   /*** Input validation. */
/* if (NOT chxcheck (x, "chxvalue"))  RETURN (-1L);   */

   if (x == nullchix)
       { chxerror (CXE_NULL,"chxvalue", "");        RETURN(-1L); }
   if (FREED(x))    
       { chxerror (CXE_FREED,"chxvalue", x->name);  RETURN(-1L); }
   if (NOT ISCHIX(x))
       { chxerror (CXE_NOTCHX,"chxvalue", "");      RETURN(-1L); }
   
   if (0 >  x->actlen)                chxlen (x);
   if (n >= x->actlen)                RETURN ( 0L);
   
#if USE_THIN_CHIX
   RETURN (x->p[n]); 

#else
   p = x->p + x->type * n;
   v = *p++ & 0xFF;
   for (i=1;   i<x->type;   ++i)  { v = v << 8;   v += *p++; }
   RETURN (v);
#endif

}
