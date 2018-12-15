
/*** CHXREPVAL.  Replace individual values in a chix.
/
/    chxrepval (x, pos, width, oldval, newval)
/
/    Parameters:
/       Chix  x;      (chix to work with)
/       int4  pos;    (replace all values at/after this position)
/       int   width;  (THIN or WIDE of new value)
/       int4  oldval; (replace all of these...)
/       int4  newval; (...with this)
/
/    Purpose:
/       Replace individual character values with a new value.
/
/    How it works:
/       NEWVAL is assumed to be a character of type WIDTH.  If
/       X is of a smaller type, it is promoted to WIDTH.
/
/    Returns: 1 on success
/             0 on error
/   
/    Error Conditions:  X not a chix.
/  
/    Known bugs:
/       Limitation: requires that a character in a chix be no more than 
/       sizeof(int4) bytes, typically 4.
/
/    Speed: write 1-byte code
/
/    Home:  chx/chxrepva.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/09/97 14:05 New function. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxrepval (Chix x, int4 pos, int width, int4 oldval, int4 newval)
{
   int    i;
   int4   len, repval, val;
   uchar *p, *q;

   ENTRY ("chxrepval", "");

   /*** Input validation. */
   if (NOT chxcheck (x, "chxsetval"))  RETURN (0);
   len = chxlen(x);
   if (pos >= len)                     RETURN (0);

   if (USE_THIN_CHIX)     width = THIN;
   if (USE_WIDE_CHIX)     width = WIDE;
   if (width < THINNEST)  width = THINNEST;
   if (width > WIDEST)    width = WIDEST;
   
   /*** Try to resize X if it is narrower than WIDTH. */
   if (x->type < width  &&  NOT chxresize (x, width, len))  RETURN(0);

   /*** Starting at the POS position... */
   for (;   pos < len;   ++pos) {
      p = x->p + x->type * pos;

      /*** Compute VAL of this char. */
      q = p;
      val = *q++;
      for (i=1;   i<x->type;   ++i)  { val = val << 8;   val += (*q++ & 0xFF); }
      if  (val == 0)  break;

      /*** Replace if it's OLDVAL. */
      if (val == oldval) {
         repval = newval;
         for (i = x->type - 1;   i>=0;   --i) {
            p[i]   = repval & 0xFF;
            repval = repval >> 8;
         }
      }
   }

   RETURN (1);
}
