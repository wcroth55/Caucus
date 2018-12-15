/*** CHXWRAP.   Wrap a (multi-line) chix at a column boundary.
/
/    chxwrap (atext, maxcol);
/
/    Parameters:
/       Chix   atext;       (multi-line chix to be wrapped)
/       int    maxcol;      (max number of single-width chars per line)
/
/    Purpose:
/       'Wrap' any lines in ATEXT that are longer than MAXCOL single-width
/       characters.
/
/    How it works:
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
/    Home:  glyph/chxwrap.c
/
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/08/93 18:11 New function. */
/*: CR  1/13/93 14:57 chx int args should be int4. */
/*: CR  2/24/93 21:31 Fix comments. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"

FUNCTION  chxwrap (atext, maxcol)
   Chix   atext;
   int    maxcol;
{
   Chix   btext, line, temp;
   int4   nlpos, pos;
   int    type;

   ENTRY ("chxwrap", "");

   type  = chxtype  (atext);
   btext = chxalloc (L(500), type, "chxwrap btext");
   line  = chxalloc ( L(90), THIN, "chxwrap line");
   temp  = chxalloc ( L(90), THIN, "chxwrap temp");

   /*** Scan through ATEXT.  Wrap any too-int4 lines.  Put result in BTEXT. */
   for (nlpos=0L;   chxnextline (line, atext, &nlpos);   ) {

      /*** To determine if a line is too int4: 1-byte chix just use
      /    chxlen.  2-byte chix call chxtoowide to examine screen
      /    width of each character. */
      while (pos = (type==1 ? (chxlen(line) > maxcol ? maxcol : 0) :
                               chxtoowide (line, maxcol)) ) {
         /*** If a LINE is too int4, pull off the part (up through POS)
         /    that will fit on a line.  Add it to BTEXT.  Repeat. */
         chxcpy     (temp,  line);
         chxcatsub  (btext, temp, L(0), pos);
         chxcatval  (btext, THIN, L('\n'));
         chxclear   (line);
         chxcatsub  (line,  temp, pos, ALLCHARS);
      }

      chxcat    (btext, line);
      chxcatval (btext, THIN, L('\n'));
   }

   chxcpy  (atext, btext);
   chxfree (btext);
   chxfree (line);
   chxfree (temp);

   RETURN  (1);
}
