
/*** CHXTRUNC.   Truncate a chix at position POS.
/
/    chxtrunc (x, pos);
/
/    Parameters:
/       Chix  x;      (chix to work with)
/       int4  pos;    (truncate at this char position)
/
/    Purpose:
/       Truncate chix X at position POS.
/
/    How it works:
/
/    Returns: 1
/   
/    Error Conditions:  X not a chix.
/  
/    Known bugs:
/
/    Home:  chx/chxtrunc.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/06/95 16:28 New function. */
/*: OQ  3/08/98 added new code for single  byte character chix type */
 
#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxtrunc (x, pos)
   Chix   x;
   int4   pos;
{
   int    i;
   int4   len;
   uchar *p;

   ENTRY ("chxtrunc", "");

   /*** Input validation. */
   /*if (NOT chxcheck (x, "chxtrunc"))               RETURN (0);*/
   if (x == nullchix)
      { chxerror (CXE_NULL, "chxtrunc", "");         RETURN(0); }
   if (FREED(x))    
      { chxerror (CXE_FREED, "chxtrunc", x->name);   RETURN(0); }
   if (NOT ISCHIX(x))
      { chxerror (CXE_NOTCHX, "chxtrunc", "");       RETURN(0); }

   len = chxlen(x);
   if (pos >= len)                                   RETURN (0);

#if USE_THIN_CHIX
   x->p[pos] = '\0';

#else
   p = x->p + x->type * pos;

   for (i = x->type - 1;   i>=0;   --i)   p[i] = 0;
#endif

   x->actlen = pos;

   RETURN (1);
}
