/*** LIMIT_10K.   Limit a chix to approximately 10K lines.
/
/    big = limit_10k (atext);
/
/    Parameters:
/       int    big;         (was chix > 10,000 lines? )
/       Chix   atext;       (multi-line chix to be limited)
/
/    Purpose:
/       Truncate any chix that is longer than approx 10,000 lines.
/
/    How it works:
/       Actually truncates chix with more than 'LIMIT' lines.
/
/    Returns: 1 if the chix was > 10,000 lines
/             0 otherwise
/
/    Error Conditions:
/  
/    Known bugs:      none
/
/    Home:  glyph/limit10k.c
/
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/06/95 16:17 New function. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"

#define  LIMIT   L(9980)

FUNCTION  limit_10k (atext)
   Chix   atext;
{
   Chix   line;
   int4   nlpos, lines, lastpos;
   int    big;

   ENTRY ("limit10k", "");

   line  = chxalloc ( L(90), THIN, "chxwrap line");
   big   = 0;

   for (nlpos=lines=0L;   chxnextline (line, atext, &nlpos);   ++lines) {
      if (lines >= LIMIT) {
         chxtrunc (atext, lastpos);
         big = 1;
         break;
      }
      lastpos = nlpos;
   }

   chxfree (line);
   RETURN  (big);
}
