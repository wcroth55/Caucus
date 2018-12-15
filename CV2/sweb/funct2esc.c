
/*** FUNC_T2ESC.   Escape all instances of "&", "<", and ">".
/
/    func_t2esc (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       The browser display process "unescapes" all "&" sequences
/       such as "&gt;", "&amp;", etc.
/
/       If raw HTML text that includes such sequences is going to
/       be displayed in a text box to be edited by the user, this
/       "unescaping" causes a problem -- any original escaped sequences
/       enter by the user will be lost!  Furthermore, some browsers
/       (e.g., Internet Explorer, grumble, grumble) will not display
/       HTML tags at all(!) inside a text box.
/
/       Therefore func_t2esc() is provided, to "pre-escape" all 
/       instances of "&", "<", and ">".  The resultant "unescaping" 
/       of the "pre-escaped" sequences thus yields the original sequence!
/
/       Func_t2esc() acts as a simple output filter, reading ARG,
/       escaping any such sequences, and appending the result to
/       RESULT.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funct2hbr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/08/96 23:31 New function */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  func_t2esc (Chix result, Chix arg)
{
   static Chix amp = (Chix) NULL;
   static Chix escamp, lt, esclt, gt, escgt, temp;
   int4   pos, value;
   int    type;
   ENTRY ("func_t2esc", "");

   type = chxtype (arg);

   if (amp == (Chix) NULL) {
      amp    = chxalsub (CQ("&"),     L(0), L(10));
      escamp = chxalsub (CQ("&amp;"), L(0), L(10));
      lt     = chxalsub (CQ("<"),     L(0), L(10));
      esclt  = chxalsub (CQ("&lt;"),  L(0), L(10));
      gt     = chxalsub (CQ(">"),     L(0), L(10));
      escgt  = chxalsub (CQ("&gt;"),  L(0), L(10));
      temp   = chxalloc (L(512), type, "func_t2amp temp");
   }

   chxcpy     (temp, arg);
   chxreplace (temp, L(0), amp, escamp);
   chxreplace (temp, L(0), lt,  esclt);
   chxreplace (temp, L(0), gt,  escgt);

   chxcat     (result, temp);

   RETURN (1);
}
