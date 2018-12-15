
/*** CHXCATVAL.  Concatenate a single character value onto the end of a chix.
/
/    chxcatval (a, width, v);
/
/    Parameters:
/       Chix  a;
/       int   width;   (THIN or WIDE)
/       int4  v;       (value of a single character to add to end of chix A)
/
/    Purpose:
/       Convert int4 value V into a single chix character, of size WIDTH, 
/       and concatenate onto the end of A.  (If type of A < WIDTH, resize
/       A to WIDTH.)
/
/    How it works:
/
/    Returns: 1 on success, 0 on error.
/
/    Error Conditions:  A not a chix (calls chxerror() to complain)
/                       A cannot be expanded if needed.
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
/    Speed: 
/
/    Home:  chx/chxcatv.c.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR  1/03/92 13:30 Return 0 on error. */
/*: CR 12/11/92 13:19 Chxresize last arg is int4 *. */
/*: CR  3/15/95 20:29 Add USE_WIDE_CHIX. */
/*: OQ 02/09/98 replace call to chxcheck by on line commands */
/*: OQ 02/24/98 added code for single-byte character chix type */
/*: CR 06/23/98 speed: else if's; in-line replacement for chxlen() */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxcatval (a, width, v)
   Chix   a;
   int    width;
   int4   v;
{
   uchar *z, *p;
   int    i, n, j, sum;
   int4   len;

   ENTRY ("chxcatval", "");

   /*** Input validation. */
   /*if (NOT chxcheck (a, "chxcatval"))      RETURN(0);*/
   if (a == nullchix)
      { chxerror (CXE_NULL, "chxcatval", "");          RETURN(0); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxcatval", a->name);    RETURN(0); }
   if (NOT ISCHIX(a))
      { chxerror (CXE_NOTCHX, "chxcatval", "");        RETURN(0); }
   
   if      (USE_THIN_CHIX)     width = THIN;
   else if (USE_WIDE_CHIX)     width = WIDE;
   else if (width < THINNEST)  width = THINNEST;
   else if (width > WIDEST)    width = WIDEST;
   
   if ((len = a->actlen) < 0)  len = chxlen (a);

   /*** Ensure A is big enough, in size and type, for the new character. */
   if (
#if ! USE_THIN_CHIX
       a->type < width  ||  
#endif
       a->maxlen <= len+2) {

      if (NOT chxresize (a, width, len+5))  RETURN(0);
   }

   /*** Point Z to the end of A, and add the new character. */
#if USE_THIN_CHIX
   z = a->p + len;
  *z = v & 0xFF;

   /*** Point Z after the new character, and put in the zero terminator. */
   z++;
  *z = 0;

#else
   z = a->p + a->type * len;
   for (i = a->type - 1;   i>=0;   --i) {
      z[i] = v & 0xFF;
      v    = v >> 8;
   }

   /*** Point Z after the new character, and put in the zero terminator. */
   z += a->type;
   for (i=0;   i < a->type;   ++i)   z[i] = 0;
#endif
   a->actlen++;

   RETURN (1);
}
