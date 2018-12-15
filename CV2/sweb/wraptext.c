
/*** WRAP_TEXT.
/
/    wrap_text (result, arg);
/   
/    Parameters:
/       Chix    result;  (append result here)
/       Chix    arg;     (function argument)
/
/    Purpose:
/       Do "ordinary" word-wrapping of text.
/
/    How it works:
/       Implements CML function $wraptext (width text).  WIDTH is
/       the number of single-width character cells.
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  wraptext.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/17/96 16:16 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  wrap_text (Chix result, Chix arg)
{
   int4   pos, start, cval, wrap;
   int    width, blank, size;
   ENTRY ("wrap_text", "");

   pos  = L(0);
   wrap = chxint4 (arg, &pos);
   if (wrap < 5)  wrap = 5;

   blank = -1;
   width =  0;
   for (start = ++pos;   cval = chxvalue (arg, pos);   ++pos) {
      
      if (cval == '\n') {
         chxcatsub (result, arg, start, pos-start+1);
         start = pos + 1;
         width =  0;
         blank = -1;
         continue;
      }

      size = jixwidth (cval);

      if (width + size > wrap) {
         if (blank < 0)  blank = start + width/2 - 3;
         chxcatsub (result, arg, start, blank-start+1);
         chxcatval (result, THIN, L('\n'));
         start = blank + 1;
         pos   = blank;
         width =  0;
         blank = -1;
      }

      else {
         if (cval==' '  ||  cval==41377L  || cval=='\t')  blank = pos;
         width += size;
      }
   }
   chxcatsub (result, arg, start, pos-start);

   RETURN (1);
}
