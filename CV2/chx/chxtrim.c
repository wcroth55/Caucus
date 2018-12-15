
/*** CHXTRIM.   Trim trailing blanks(?) from a chix.
/
/    chxtrim (x);
/
/    Parameters:
/       Chix  x;   
/
/    Purpose:
/       Remove any blanks at the end of chix X.
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
/    Known bugs:
/       This version only knows about the regular ascii blank character
/       " ".  It should probably use the chxspecial BLANK stuff.
/
/    Home:  chx/chxtrim.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/15/91 18:53 New function. */
/*: OQ 02/19/98 replace chxcheck by new codes */
/*: OQ 03/07/98 added new code for single byte character chix byte */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxtrim (x)
   Chix   x;
{
   int4   v;
   int    i, last;
   uchar *p;

   ENTRY ("chxtrim", "");

   /*** Input validation. */
   /*if (NOT chxcheck (x, "chxtrim"))                RETURN(0);*/
   if (x == nullchix)
      { chxerror (CXE_NULL, "chxtrim", "");          RETURN(0); }
   if (FREED(x))    
      { chxerror (CXE_FREED, "chxtrim", x->name);    RETURN(0); }
   if (NOT ISCHIX(x))
      { chxerror (CXE_NOTCHX, "chxtrim", "");        RETURN(0); }

   /*** Find the position of the LAST character. */
   last = (x->actlen >= 0 ? x->actlen : chxlen (x)) - 1;

   /*** While that character is a blank, back up one. */
#if USE_THIN_CHIX
   for (p=x->p;   last >= 0;   --last) {
      if (p[last] != ' ')  break;
   }
   p[++last] = '\0';

#else
   for (;   last >= 0;   --last) {
      p = x->p + x->type * last;
      v = *p++ & 0xFF;
      for (i=1;   i<x->type;   ++i)  { v = v << 8;   v += (*p++ & 0xFF); }
      if  (v != ' ')  break;
   }

   ++last;
   p = x->p + x->type * last;
   for (i=0;   i<x->type;   ++i)  *p++ = 0;
#endif
   x->actlen = last;

   RETURN (1);
}
