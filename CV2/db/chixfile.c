/*** CHIXFILE.   Load a Caucus text file into a chix.
/
/    ok = chixfile (n, a, prefix);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    n;           (unit code of file to be loaded)
/       Chix   a;           (load file into A)
/       char  *prefix;      (prefix each line with this string)
/
/    Purpose:
/       Load a text file into a chix string.
/
/    How it works:
/       Open the text file (already locked by the caller) on unit N,
/       and copy it into chix A.  A must already be allocated by the
/       caller.  Each line copied into A is cleaned, tabs are translated
/       to blanks, trimmed of trailing blanks, and prefixed with the string
/       PREFIX.
/
/    Returns: 1 on success
/             0 if file N cannot be opened.
/            -1 if only part of file N could be read.
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
/    Home:  db/chixfile.c
/
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/25/91 13:59 New function. */
/*: CR  7/08/91 12:05 Add PREFIX argument. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR  1/03/92 14:18 Return -1 if only part of the file could be read. */
/*: JX  5/26/92 14:41 Clean. */
/*: CR  1/08/93 17:16 Add cleaning, tab replacement, etc. on TEMP. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  chixfile (n, a, prefix)
   int    n;
   Chix   a;
   char  *prefix;
{
   Chix   temp;
   int    more;
   Chix   endline, begline;

   ENTRY ("chixfile", "");

   if (NOT unit_view (n))  RETURN (0);

   temp    = chxalloc (L(200), THIN, "chixfile temp");
   endline = chxalloc (L(10),  THIN, "chixfile endline");
   begline = chxalloc (L(10),  THIN, "chixfile begline");

   /*** Convert the end-of-line and beginning-of-line strings into Chix. */
   chxcatval (endline, THIN, L('\n'));
   chxcpy (begline, chxquick (prefix, 0));

   /*** Read and convert/assemble lines from the file. */
   chxclear (a);
   while (more = unit_read (n, temp, L(0))) {
      sysclean (temp);
      fixtabs  (temp);
      chxtrim  (temp);

      chxcat (a, begline);
      if (NOT chxcat (a, temp))  break;
      chxcat (a, endline);
   }
   unit_close (n);

   chxfree (temp);
   chxfree (begline);
   chxfree (endline);

   RETURN  (more ? -1 : 1);
}
