/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** STR36TOI.   Convert base 36 string into a int4. 
/
/   int4 n = str36toi (str)
/
/   Parameters:
/      int4  n;      (int4 value of STR)
/      char *str;    (base 36 string that you want to interpret)
/
/   Purpose:
/      Some of the Caucus database files have names that end in a 4-digit
/      sexatridecimal (STD: base 36) part, e.g. "SHIP001/H001M38X".
/
/      STR36TOI()takes a string of characters in STD and translates them
/      into one int4 integer.
/
/      STR36TOI() doesn't handle negative numbers.
/
/   How it works:
/
/   Returns:
/      See Purpose.
/
/   Error Conditions: None.  Note the following maximums for given string
/      sizes:
/                  STR size            Max STD (Decimal)
/                  --------            -------
/                      3                46,656
/                      4             1,679,616
/                      7        78,364,164,096
/
/   Related Functions: strito36()
/
/   Called by:
/
/   Home: str/str36toi.c
*/

/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV 14:10/91 14:10 Clean up header. */

static char *alphanums = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

#include "int4.h"

int4 str36toi (str)
   char  str[];
{
   int   i, j;
   int4  num=0;
   char *s;

   s = str;
   for (;   *s;   ++s)   if (cf_lower(*s))    *s += ('A' - 'a');
   for (i=0;  str[i] > 47 && str[i] <58 || str[i] > 64 && str[i] < 91; ++i) {
      for (j=0;   alphanums[j]  &&  alphanums[j] != str[i];   ++j) ;
      if  (! alphanums[j])  return (-1);

      num = 36 * num + j;
   }

   return (num);
}
