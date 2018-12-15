/*** CHXTOOWIDE.
/
/    col = chxtoowide (line, maxcol);
/
/    Parameters:
/       Chix   line;        (line to be tested)
/       int    maxcol;      (max number of single-width chars per line)
/
/    Purpose:
/       Detect if a LINE is wider than MAXCOL.  Handles double-width
/       jix as well.
/
/    How it works:
/
/    Returns: 
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  glyph/chxtoowi.c
/
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/29/95 15:52 New file, extracted from old chxwrap.c */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"


/*** Is chix LINE too wide (> maxcol?).  If so, return character
/    position that takes width beyond maxcol. */

FUNCTION  chxtoowide (line, maxcol)
   Chix   line;
   int    maxcol;
{
   int4   value, i;
   int    width;

   for (width=i=0;   value = chxvalue (line, i);   ++i) {
      width += jixwidth (value);
      if (width > maxcol) return (i);
   }

   return (0);
}
