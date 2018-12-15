
/*** FUNC_ESCQUOTE.   Escape all double quotes (as "&quot;").
/
/    func_escquote (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       It is sometimes necessary to "escape" all double-quotes,
/       i.e. translate them into the special HTML sequence &quot;
/
/       This is particularly true for data inside double quotes
/       in the VALUE field of <INPUT> tags.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/funcescq.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/22/97 13:31 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  func_escquote (Chix result, Chix arg)
{
   static Chix quote = (Chix) NULL;
   static Chix escqt, temp;
   int    type;
   ENTRY ("func_escquote", "");

   type = chxtype (arg);

   if (quote == (Chix) NULL) {
      quote = chxalsub (CQ("\""),     L(0), L(10));
      escqt = chxalsub (CQ("&quot;"), L(0), L(10));
      temp  = chxalloc (L(512), type, "func_escq temp");
   }

   chxcpy     (temp, arg);
   chxreplace (temp, L(0), quote, escqt);
   chxcat     (result, temp);

   RETURN (1);
}
