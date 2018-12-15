
/*** FUNC_ESCSINGLE.   Escape all single quotes (as "\'").
/
/    func_escsingle (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       It is sometimes necessary to "escape" all single-quotes,
/       i.e. translate them into the special sequence "\'".
/
/       This is particularly true for data inside single quotes
/       in in-line javascript functions or function calls.
/
/    How it works:
/       Also translates newlines into '\n'.
/       Also translates returns  into '\r'.
/
/       If something has already been escaped, leave it alone!
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/funcescs.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/25/02 New function. */
/*: CR 01/08/03 Add newline translation. */
/*: CR 07/05/04 Add return  translation. */
/*: CR 02/07/05 Do not escape things that are already escaped! */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  func_escsingle (Chix result, Chix arg)
{
   static Chix quote = (Chix) NULL;
   static Chix escqt, temp, newl, escnl, crtn, esccr;
   int    type, c, pos;
   ENTRY ("func_escsingle", "");

   type = chxtype (arg);

/*** Version 1 
   if (quote == (Chix) NULL) {
      quote = chxalsub (CQ("'"),   L(0), L(10));
      escqt = chxalsub (CQ("\\'"), L(0), L(10));
      newl  = chxalsub (CQ("\n"),  L(0), L(10));
      crtn  = chxalsub (CQ("\r"),  L(0), L(10));
      escnl = chxalsub (CQ("\\n"), L(0), L(10));
      esccr = chxalsub (CQ("\\r"), L(0), L(10));
      temp  = chxalloc (L(512), type, "func_escs temp");
   }

   chxcpy     (temp, arg);
   chxreplace (temp, L(0), quote, escqt);
   chxreplace (temp, L(0), newl,  escnl);
   chxreplace (temp, L(0), crtn,  esccr);
   chxcat     (result, temp);
*/

#define ADD(x)  chxcatval (result, type, ((int4) x))

   for (pos=0;   (c = chxvalue (arg, pos));   ++pos) {
      if      (c == '\'')  { ADD('\\');   c = '\''; }
      else if (c == '\n')  { ADD('\\');   c = 'n';  }
      else if (c == '\r')  { ADD('\\');   c = 'r';  }
      else if (c == '\\')  { 
         ADD(c);
         c = chxvalue (arg, ++pos);
      }
      ADD(c);
   }

   RETURN (1);
}
