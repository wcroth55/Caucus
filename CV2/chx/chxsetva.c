
/*** CHXSETVAL.  Set the value of a selected character in a chix.
/
/    chxsetval (x, pos, width, val);
/
/    Parameters:
/       Chix  x;      (chix to work with)
/       int4  pos;    (position of character in X)
/       int   width;  (THIN or WIDE)
/       int4  val;    (value to set that character to)
/
/    Purpose:
/       Set the value of the POS'th character in chix X to VAL.
/
/    How it works:
/       VAL is assumed to be a character of type WIDTH.  If
/       X is of a smaller type, it is promoted to WIDTH.
/
/    Returns: 1 on success
/             0 on error
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
/       Limitation: requires that a character in a chix be no more than 
/       sizeof(int4) bytes, typically 4.
/
/    Home:  chx/chxsetva.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/20/92 17:32 New function. */
/*: CR 12/11/92 13:22 Chxresize last arg is int4 *. */
/*: CR 12/11/92 13:28 POS arg is long. */
/*: CR  3/15/95 20:29 Add USE_WIDE_CHIX. */
/*: OQ  2/18/98 replace chxchech by in line code */
/*: OQ  3/04/98 added new code for single byte character chix type */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxsetval (x, pos, width, val)
   Chix   x;
   int4   pos;
   int    width;
   int4   val;
{
   int    i;
   int4   len;
   uchar *p;

   ENTRY ("chxsetval", "");

   /*** Input validation. */
   /*if (NOT chxcheck (x, "chxsetval"))               RETURN (0);*/
   if (x == nullchix)
      { chxerror (CXE_NULL, "chxsetval", "");         RETURN(0); }
   if (FREED(x))    
      { chxerror (CXE_FREED, "chxsetval", x->name);   RETURN(0); }
   if (NOT ISCHIX(x))
      { chxerror (CXE_NOTCHX, "chxsetval", "");       RETURN(0); }

   len = chxlen(x);
   if (pos >= len)                     RETURN (0);

   if (USE_THIN_CHIX)     width = THIN;
   if (USE_WIDE_CHIX)     width = WIDE;
   if (width < THINNEST)  width = THINNEST;
   if (width > WIDEST)    width = WIDEST;
   
   /*** Try to resize X if it is narrower than WIDTH. */
   if (x->type < width  &&  NOT chxresize (x, width, len))  RETURN(0);

#if USE_THIN_CHIX
   p = x->p + pos;
  *p = val & 0xFF;
  
#else
   p = x->p + x->type * pos;
   for (i = x->type - 1;   i>=0;   --i) {
      p[i] = val & 0xFF;
      val  = val >> 8;
   }
#endif

   RETURN (1);
}
