
/*** FUNC_T2MAIL.  Detect e-mail addresses and turn them into "mailto" URLs. 
/
/    func_t2mail (arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/
/    Purpose:
/       Output filter: detect e-mail addresses in text, and turn
/       them into "mailto" URLs.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funct2mail.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/04/96 19:34 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  func_t2mail (Chix arg)
{
   static Chix atsign = nullchix;
   static Chix temp, word;
   int4   pos, where;
   ENTRY ("func_t2mail", "");

   if (atsign == nullchix)  {
      atsign = chxalsub (CQ("@"), 0, 4);
      temp   = chxalloc (L(60), THIN, "funct2mail temp");
      word   = chxalloc (L(60), THIN, "funct2mail word");
   }

   for (pos=0;   (where = chxnextword (word, arg, &pos)) >= 0;  ) {

      if (chxindex (word, L(0), atsign) <= 0)  continue;

      chxcpy   (temp, CQ("<A HREF=\"mailto:"));
      chxcat   (temp, word);
      chxcat   (temp, CQ("\">"));
      chxcat   (temp, word);
      chxcat   (temp, CQ("</A>"));
      chxalter (arg, where, word, temp);
      pos += chxlen(temp) - chxlen(word);
   }

   RETURN (1);
}
