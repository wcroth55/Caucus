
/*** CHXRESIZE.  Resize the data area of a chix.
/
/    chxresize (x, newtype, newmax);
/
/    Parameters:
/       Chix  x;
/       int   newtype;
/       int4  newmax;
/
/    Purpose:
/       Resize the data area of chix X, to (at least) NEWMAX characters of
/       type NEWTYPE.  The old data is copied (and converted, if necessary)
/       into the new data area.
/
/       The new data is at least as large, in size and type, as the old
/       data area, regardless of the values of NEWTYPE and NEWMAX.
/
/    How it works:
/       Chxresize allocates a new data area with chxmem(), copies the
/       old data into it, resets X's data area pointer to point to the
/       new area, and frees the old data area.
/
/       Reasonable values are enforced for NEWTYPE and NEWMAX.
/
/    Returns: 1 on success, 0 on error.
/
/    Error Conditions:  x is not a proper chix.
/                       NEWMAX is too big (> 32760)
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  chx...() functions only, never by application.
/
/    Operating system dependencies:
/       Uses sysfree() to free memory allocated by sysmem().
/
/    Known bugs:      none
/
/    Home:  chx/chxresiz.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR 10/07/91 17:38 Always expand by at least 1.5 old max. */
/*: CR  1/03/92 13:22 Limit resizing to ALLCHARS max. */
/*: CR 12/11/92 11:05 chxcapc last arg is long. */
/*: CR  3/15/95 20:16 Never shrink chix! */
/*: CR  3/15/95 20:29 Add USE_WIDE_CHIX. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxresize (x, newtype, newmax)
   Chix   x;
   int    newtype;
   int4   newmax;
{
   uchar *p0, *p1;
   int4   expand;
   static FILE *log = NULL;
   FILE  *fopen();

   ENTRY ("chxresize", "");

   /*** Input verification. */
   if (NOT chxcheck (x, "chxresize"))  RETURN (0);
   if (USE_THIN_CHIX)        newtype = THIN;
   if (USE_WIDE_CHIX)        newtype = WIDE;
   if (newtype < THINNEST)   newtype = THINNEST;
   if (newtype > WIDEST)     newtype = WIDEST;
   if (newtype < x->type)    newtype = x->type;
   if (newmax  < 2)          newmax  = 2;

   /*** Don't "shrink" chix! */
   if (newtype == x->type  &&  newmax < x->maxlen)  RETURN (1);
   if (newmax  <  x->maxlen)   newmax = x->maxlen;

   /*** Use this commented out code to watch odd behaviour
   /    by chxresize(). */
/*
   if (log == NULL) log = fopen ("resize.log", "a");
   fprintf (log, "Resize: '%-20s'  %5d(%d) -> %5d(%d)\n", 
                  x->name, x->maxlen, x->type, newmax, newtype);
*/

   /*** Always expand to at least 1.5 times old max len. */
   expand = (3L * x->maxlen) / 2L;
   if (newmax < expand)                newmax = expand;
   
   /*** P0 is the "old" character buffer, P1 the "new" buffer. */
   p0 = x->p;
   p1 = chxmem ( (newmax+1) * newtype, x->name);

   /*** Copy (and promote if necessary) the characters from P0 to P1. */
   chxcapc (p1, newtype, p0, x->type, -1L);

   x->maxlen = newmax;
   x->type   = newtype;
   x->p      = p1;
   sysfree ( (char *) p0);

   /*** Warn user if this chix is resized too often. */
   if (++x->resize > RESIZE_LIM) {
      chxerror (CXE_RESIZE, "chxresize", x->name);
      x->resize = 0;
   }

   RETURN  (1);
}
