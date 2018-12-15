
/*** FUNC_T2HBR.  Filter text to HTML, with line breaks.
/
/    func_t2hbr (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Output filter: translate text (with newlines) into HTML
/       (with <BR> line breaks).
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

/*: CR  3/29/95 16:22 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  func_t2hbr (Chix result, Chix arg)
{
   static Chix br = (Chix) NULL;
   static Chix lt, gt, amp, quot, bold1, bold0;
   int4   pos, value;
   int    type;
   ENTRY ("func_t2hbr", "");

   if (br == (Chix) NULL) {
      br    = chxalsub (CQ("<BR>\n"), L(0), L(10));
      lt    = chxalsub (CQ("&lt;"),   L(0), L(10));
      gt    = chxalsub (CQ("&gt;"),   L(0), L(10));
      amp   = chxalsub (CQ("&amp;"),  L(0), L(10));
      quot  = chxalsub (CQ("&quot;"), L(0), L(10));
      bold1 = chxalsub (CQ("<B>"),    L(0), L(10));
      bold0 = chxalsub (CQ("</B>"),   L(0), L(10));
   }

   type = chxtype (arg);
   for (pos=0L;   (value = chxvalue (arg, pos));   ++pos) {
      if      (value == '\n')   chxcat    (result, br);
      else if (value == '<')    chxcat    (result, lt);
      else if (value == '>')    chxcat    (result, gt);
      else if (value == '&')    chxcat    (result, amp);
      else if (value == '"')    chxcat    (result, quot);
      else if (value ==  1 )    chxcat    (result, bold1);
      else if (value ==  2 )    chxcat    (result, bold0);
      else                      chxcatval (result, type, value);
   }

   RETURN (1);
}
