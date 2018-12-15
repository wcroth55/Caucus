
/*** FUNC_BIT.   Handle $bit_xxx()  functions.
/
/    func_bit   (result, what, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       char               *what;    (function name)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Handle bit-logic related functions.
/       Append value of bit_xxx() function WHAT with arguments ARG to RESULT.
/
/    How it works:
/       $bit_or  (a b ...)
/       $bit_and (a b ...)
/       $bit_not (a)
/
/    Returns: void
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/funcbit.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/02/97 16:48 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  func_bit (Chix result, char *what, Chix arg)
{
   int4  val1, val2, pos, pos2;
   char  temp[100];
   ENTRY ("func_bit", "");

   if (streq (what, "bit_or")) {
      val1 = val2 = 0;
      pos  = pos2 = L(0);
      do {
         pos  = pos2;
         val1 = val1 | val2;
         val2 = chxint4 (arg, &pos2);
      } while (pos2 != pos);

      sprintf (temp, "%d", val1);
      chxcat  (result, CQ(temp));
   }

   else if (streq (what, "bit_and")) {
      val1 = val2 = -1;
      pos  = pos2 = L(0);
      do {
         pos  = pos2;
         val1 = val1 & val2;
         val2 = chxint4 (arg, &pos2);
      } while (pos2 != pos);

      sprintf (temp, "%d", val1);
      chxcat  (result, CQ(temp));
   }

   else if (streq (what, "bit_not")) {
      val1 = 0;
      pos  = L(0);
      val1 = chxint4 (arg, &pos);
      sprintf (temp, "%d", (~val1) & 0xFFFF);
      chxcat  (result, CQ(temp));
   }

   RETURN(1);
}
