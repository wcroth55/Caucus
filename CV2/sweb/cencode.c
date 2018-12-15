
/*** CENCODE.  Encode single characters in "%xx" form.
/
/    cencode (result, cval, arg)
/   
/    Parameters:
/       Chix  result;  (append resulting value here)
/       int   cval;    (character value to be encoded)
/       Chix  arg;     (original string to be encoded)
/
/    Purpose:
/       Encode all of a given character in a string in the web-like
/       form "%xx", where "xx" is the hexadecimal value of the character.
/
/    How it works:
/       Translates all characters CVAL in ARG into "%xx" form; appends the
/       complete result to RESULT.
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/cencode.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/21/00 17:53 New function */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

FUNCTION  cencode (Chix result, int cval, Chix arg)
{
   static Chix coded = (Chix) NULL;
   int4   pos, cv;
   int    type;
   char   temp[80];
   ENTRY ("cencode", "");

   if (coded == (Chix) NULL) {
      sprintf (temp, "%%%02x", cval);
      coded = chxalsub (CQ(temp), L(0), L(10));
   }

   type = chxtype (arg);

   for (pos=0;   (cv = chxvalue (arg, pos));   ++pos) {
      if (cv == cval) chxcat    (result, coded);
      else            chxcatval (result, type, cv);
   }

   RETURN (1);
}
