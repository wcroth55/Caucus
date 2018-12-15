/*** COL_WRAP.   Wrap a (multi-line) chix at a column boundary.
/
/    col_wrap (result, source, start, width);
/
/    Parameters:
/       Chix   result;      (where to put the resulting wrapped chix)
/       Chix   source;      (source of text)
/       int4   start;       (start at this position in SOURCE)
/       int    width;       (word-wrap up to this column)
/
/    Purpose:
/       Word-wrap any lines in SOURCE that are longer than WIDTH 
/       single-width characters, and place the result in RESULT.
/
/    How it works:
/       If WIDTH is 0, just copies SOURCE[start] to RESULT.
/
/    Returns: 1
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
/    Home:  glyph/colwrap.c
/
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/29/95 15:25 New function. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"

FUNCTION  col_wrap (result, source, start, width)
   Chix   result, source;
   int4   start;
   int    width;
{
   Chix   line, temp;
   int4   nlpos, pos;
   int    type, half, back, backup;

   ENTRY ("col_wrap", "");

   /*** Handle special case: no wrapping. */
   if (width <= 0) {
      chxcatsub (result, source, start, ALLCHARS);
      RETURN (1);
   }

   type  = chxtype  (source);
   line  = chxalloc ( L(90), THIN, "col_wrap line");
   temp  = chxalloc ( L(90), THIN, "col_wrap temp");

   /*** There are some oddities about what happens when a int4
   /    (unbroken by blanks) string of english vs. japanese
   /    characters are encountered.  Based on experiment, we use
   /    different values for "backing up" from the column wrapping
   /    depending on the language (character set) in use... */
   backup = (type==1 ? 2 : 1);

   /*** Pluck each LINE out of SOURCE... */
   for (nlpos=start;   chxnextline (line, source, &nlpos);   ) {

      /*** And while it is too int4 (1-byte chix use chxlen;
      /    2-byte chix call chxtoowide to examine screen
      /    width of each character), wrap it. */
      while (pos = (type==1 ? (chxlen(line) > width ? width : 0) :
                               chxtoowide (line, width)) ) {

         /*** Go backwards, looking for a space.  Give up at 2/3
         /    of the line length, and chop it arbitrarily. */
         /*** (This should probably also handle a double-width
         /    space somehow.) */
         for (back=1, half=pos/3;   back < half;   ++back) {
            if (chxvalue (line, pos-back) == ' ')  break;
         }
         if (back >= half)  back = backup;

         /*** Pluck out the appropriately wrapped section.  Include
         /    the blank (and newline) on the end. */
         chxcatsub (result, line, L(0), pos-back+1);
         chxcatval (result, THIN, L('\n'));

         chxcpy    (temp, line);
         chxclear  (line);
         chxcatsub (line, temp, pos-back+1, ALLCHARS);
      }

      chxcat    (result, line);
      chxcatval (result, THIN, L('\n'));
   }

   chxfree (line);
   chxfree (temp);

   RETURN  (1);
}
