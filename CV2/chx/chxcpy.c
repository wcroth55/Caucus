
/*** CHXCPY.  Copy one chix into another.
/
/    chxcpy (a, b);
/
/    Parameters:
/       Chix  a, b;
/
/    Purpose:
/       Copy chix B into chix A, overwriting old contents of A.
/
/    How it works:
/       If type(B) > type(A), A is promoted.
/       If type(A) > type(B), B's data is promoted as it is copied.
/       B is never changed.
/
/    Returns: 1
/
/    Error Conditions:  none
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
/    Speed: use chxcapc() instead of chxcat.  Use memcpy instead of loop.
/
/    Home:  chx/chxcpy.c.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR  7/08/91 15:31 Fix chxcheck() call. */
/*: CR 10/20/91 11:54 Copy dic field as well. */
/*: CR 12/11/92 13:20 Chxresize last arg is int4 *. */
/*: CR  3/15/95 20:08 Fix bug when type(A) > type(B) ! */
/*: OQ 02/09/98 replace the call to chxcheck by in line commands*/
/*: OQ 02/26/98 added code for single-byte character chix type */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxcpy (a, b)
   Chix   a, b;
{
   int4   len, bytes, i;

   ENTRY ("chxcpy", "");

   /*if (NOT chxcheck (a, "chxcpy")  ||  NOT chxcheck (b, "chxcpy"))  RETURN(1);*/
   if (a == nullchix || b == nullchix )
      { chxerror (CXE_NULL, "chxcpy" , "");        RETURN(1); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxcpy" , a->name);  RETURN(1); }
   if (FREED(b))    
      { chxerror (CXE_FREED,  "chxcpy" , b->name); RETURN(1); }
   if ( (NOT ISCHIX(a)) || (NOT ISCHIX(b)) )
      { chxerror (CXE_NOTCHX, "chxcpy" , "");      RETURN(1); }
   

   /*** This could probably be rewritten with chxcapc() to be
   /    faster.  However, it *works* now, whereas before it
   /    ate up memory when type(a) > type (b). */

   if (a->type > b->type) {
      chxclear (a);
      chxcat   (a, b);
      RETURN   (1);
   }

   len = chxlen (b) + 1;
   if (a->type   < b->type)   chxresize (a, b->type, len+20);
   if (a->maxlen < len)       chxresize (a, a->type, len+20);

#if USE_THIN_CHIX
   for (i=0;   i<len;     ++i)  a->p[i] = b->p[i]; 
#else
   bytes = len * b->type;
   for (i=0;   i<bytes;   ++i)  a->p[i] = b->p[i];
#endif

   a->actlen = b->actlen;
   a->dic    = b->dic;

   RETURN  (1);
}
