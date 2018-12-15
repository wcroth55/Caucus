
/*** FUNC_XMLESC.   Escape all characters required by XML.
/
/    func_xmlesc (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       XML requires that the following characters be "escaped", that is,
/       translated into their XML entity equivalent:
/          original        entity
/             <            &lt;
/             >            &gt;
/             &            &amp;
/          (binary)        &#xhhhh;
/          (binary < 040)  &#xhhhh;  where hhhh = value + E000; (kludge!)
/
/       Func_xmlesc() acts as a simple output filter, reading ARG,
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
/    Home:  funcxmlesc.c
/ */

/*** Copyright (C) 1995-2007 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/16/2007 New function */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  func_xmlesc (Chix result, Chix arg)
{
   static Chix escamp = (Chix) NULL;
   static Chix esclt, escgt;
   int4   pos, cval;
   int    type;
   char   temp[16];
   ENTRY ("func_xmlesc", "");

   type = chxtype (arg);

   if (escamp == (Chix) NULL) {
      escamp = chxalsub (CQ("&amp;"), L(0), L(10));
      esclt  = chxalsub (CQ("&lt;"),  L(0), L(10));
      escgt  = chxalsub (CQ("&gt;"),  L(0), L(10));
   }

   for (pos=0;   cval = chxvalue (arg, pos);   ++pos) {
      if      (cval == '&')  chxcat    (result, escamp);
      else if (cval == '<')  chxcat    (result, esclt);
      else if (cval == '>')  chxcat    (result, escgt);
      else if (cval == 012  ||  cval == 011) chxcatval (result, type, cval);
      else if (cval < 040) {
         sprintf (temp, "&#xE0%02x;", cval);
         chxcat  (result, CQ(temp));
      }
      else if (cval > 0176) {
         sprintf (temp, "&#x%x;", cval);
         chxcat  (result, CQ(temp));
      }
      else chxcatval (result, type, cval);
   }

   RETURN (1);
}
