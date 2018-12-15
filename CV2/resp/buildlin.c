
/*** BUILDLINE.   Build up a line of text for output.
/
/    buildline (out, str, s0, s1, code);
/
/    Parameters:
/       int   out;      Unit number for output.
/       Chix  str;      Text string to be added to line being built.
/       int4  s0;       Use characters from this position...
/       int4  s1;       ... to this position, inclusive.
/       int   code;     Code for function to be performed:
/                          BLINIT    Initialize buildline() first.
/                          BLFLUSH   Flush built line to OUT.
/                          BLADD     Just add text in S to end of line.
/                          0<=n<200  Add text at column n, pad with blanks.
/                          
/    Purpose:
/       Buildline() is used to assemble text for output to unit OUT.
/       Buildline always adds the text in STR onto the "end" of the line
/       currently being assembled.  As newlines are encountered, buildline()
/       automatically writes the text to OUT.
/
/       The main reason for providing buildline at all (as opposed to
/       the caller just using unit_write() directly) is its ability to
/       add text at arbitrary column positions in the current line.
/       (The last choice for the CODE argument).  This is used
/       extensively when handling the '&z' format code in sho_format().
/
/    How it works:
/       Call buildline() with BLINIT to start.  Then add text with BLADD
/       (or n>=0) as needed.  Finish by flushing with BLFLUSH.  Next call
/       must start with BLINIT again.
/
/       STR may be nullchix, which is treated as an empty string.
/
/    Returns: 1 on success.
/             0 for bad value of CODE.
/
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  show_format() and similar "tailorable display" functions.
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  resp/buildlin.c
/           BL... codes defined in caucus.h.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/29/91 17:11 New function. */
/*: CR  5/04/91 19:17 Do not output trailing blanks at ends of lines. */
/*: CX 10/15/91 19:22 Chixify. */
/*: JX  6/17/92 17:41 Don't add STR to LINE if STR==nullchix! */
/*: JX  7/01/92 14:59 Use jixscrsize to align multiple-column data. */
/*: CR 10/11/92 22:26 Add comments. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */
/*: CR 12/11/92 17:30 Make S0, S1 int4. */
/*: CR 12/15/92 16:07 For specific CODE column value, reset P. */

#include <stdio.h>
#include "caucus.h"

#define  LINEMAX  200

FUNCTION  buildline (out, str, s0, s1, code)
   int    out, code;
   int4   s0, s1;
   Chix   str;
{
   static Chix line = nullchix;
   static Chix temp, subs;
   static int4 p;
   int4   pos;
   int    newline, maxscr, maxchar;

   ENTRY ("buildline", "");

   if (line == nullchix) {
      line = chxalloc (L(100), THIN, "buildline line");
      temp = chxalloc (L(100), THIN, "buildline temp");
      subs = chxalloc (L(100), THIN, "buildline subs");
   }

   /*** Restrict values of CODE. */
   if (code >= LINEMAX)           code = LINEMAX;

   /*** Act on CODE choice. */
   if      (code == BLINIT)     { p = 0;   chxclear (line); }
   else if (code == BLFLUSH)      ;
   else if (code == BLADD)        ;

   /*** For a specific CODE (i.e., column) value, either extend LINE
   /    out to that column with blanks, or truncate it at that column. */
   else if (code >= 0) {
      for (;   p<code;   ++p) chxcatval (line, THIN, (int4) ' ');
      p = code;
      chxcpy    (temp, line);
      chxclear  (line);
      chxcatsub (line, temp, L(0), p);
   }
      
   else  RETURN (0);


   /*** Regardless of CODE, add the requested text from STR onto this line. */
   pos = 0;
   chxclear  (subs);
   if (str != nullchix) 
      chxcatsub (subs, str, s0, (s1==ALLCHARS ? ALLCHARS : s1 - s0 + 1));
   for (pos=0;   newline = chxnextline (temp, subs, &pos);  ) {
      chxcat (line, temp);
      jixscrsize (temp, 1000, &maxchar, &maxscr);
      p +=  maxscr;
/*    p +=  chxlen (temp); */

      /*** Back up over any trailing blanks. */
      if (newline == 2) {
         chxtrim    (line);
         chxcatval  (line, THIN, (int4) '\n');
         unit_write (out, line);
         p = 0;
         chxclear   (line);
      }
   }

   /*** FLUSH empties out the current line, after we added S. */
   if (code == BLFLUSH) {
      unit_write (out, line);
      p = 0;
      chxclear   (line);
   }

   RETURN (1);
}
