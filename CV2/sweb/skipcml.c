
/*** SKIP_CML.   Skip unexecuted blocks of a CML file.
/
/    skip_cml (cfile, size, start);
/   
/    Parameters:
/       CML_File            cfile;   (CML file in memory)
/       int                 size;    (size of CML file, in # lines)
/       int                 start;   (start skipping at this line #)
/
/    Purpose:
/       A CML "for" or "if" statement may have a block that is not
/       executed.  Skip_cml() skips over this block.
/
/    How it works:
/
/    Returns: number of last line "skipped".
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  skipcml.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/27/95  9:38 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

#define DONE(x)   { rval = x;   goto done; }

FUNCTION  skip_cml (CML_File cfile, int size, int start)
{
   Chix   word;
   char   temp[100];
   int    num, rval;
   ENTRY ("skip_cml", "");

   word = chxalloc (L(30), THIN, "skip_cml word");

   for (num=start;   num<size;   ++num) {

      if (is_html (cfile->lines[num]) >= 0)  continue;

      chxtoken  (word, nullchix, 1, cfile->lines[num]);
      clean_newlines (word);
      chxtrim        (word);
      ascofchx (temp, word, L(0), 100);

      if (streq (temp, "end"))         DONE (num);

      if (streq (temp, "for")   ||
          streq (temp, "if")    ||
          streq (temp, "else")  ||
          streq (temp, "elif")  ||
          streq (temp, "while") ||
          streq (temp, "count"))       num = skip_cml (cfile, size, num+1);
   }
   DONE (size);

done:
   chxfree (word);
   RETURN  (rval);
}
