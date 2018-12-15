
/*** QUOTER.   Implement CML $quote() and $unquote() functions properly.
/
/    quoter  (result, what, arg);
/   
/    Parameters:
/       Chix                result;  (put resulting value here)
/       char               *what;    (function name)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Perform $quote() and $unquote() functions.
/
/    How it works:
/       $quote():
/          All of the "blank space" characters made known to chxspecial()
/          are encoded in the result with otherwise unused values that
/          will all be considered as part of a "word" by chxtoken and
/          related functions.
/
/       $unquote()
/          All of the "encoded" characters are restored to their
/          original values.
/
/    Returns:
/
/    Home:  quoter.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/12/98 15:33 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  quoter (
   Chix   result, 
   char  *what, 
   Chix   arg)
{
   static int   bspace[20] = {0};
   static int   ispace[20] = {1, 2,  3,  4,  5,  6, 15, 16, 17, 18, 19,
                                20, 21, 22, 23, 24, 25, 26, 0};
   Chix         blanks;
   int          num, type;
   ENTRY ("quoter", "");

   /*** Initialization.   Load up a table of the 'blank space' characters,
   /    and the internal codes (ispace) we'll use to encode them. */
   if (bspace[0] == 0) {
      blanks = chxalloc (8L, THIN, "quoter blanks");
      chxspecial (SC_GET, SC_BLANK, blanks);
      for (num=0;   ispace[num];   ++num) {
         if ( (bspace[num] = chxvalue (blanks, L(num))) <= 0)  break;
      }
      bspace[num] = 0;
      chxfree (blanks);
   }

   type = chxtype (arg);

   if (streq (what, "quote")) {
      chxcpy (result, arg);
      for (num=0;   bspace[num];   ++num) {
         chxrepval (result, L(0), type, L(bspace[num]), L(ispace[num]));
      }
   }

   else if (streq (what, "unquote")) {
      chxcpy (result, arg);
      for (num=0;   bspace[num];   ++num) {
         chxrepval (result, L(0), type, L(ispace[num]), L(bspace[num]));
      }
   }

   RETURN(1);
}
