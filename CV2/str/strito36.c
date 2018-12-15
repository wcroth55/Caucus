/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** STRITO36.    Convert int4 NUM into n-digit base 36 (0-9A-Z) string. 
/
/   char *strito36 (str, num, len)
/
/   Parameters:
/      char *str;    (The character array to store the string in.)
/      int4  num;    (The value to convert.)
/      int   len;    (The length that the result should be.)
/
/   Purpose:
/      Some of the Caucus database files have names that end in a 4-digit
/      sexatridecimal (STD: base 36) part, e.g. "SHIP001/H001M38X".
/
/      STRITO36() takes a int4 integer and converts it into a string of
/      characters in sexitridecimal (STD: based 36).  STRITO36() pads the
/      resulting string to the full length specified by LEN with zeroes.
/
/      It doesn't handle negative numbers. 
/
/   How it works:
/      The base 36 number may take more than LEN "digits"; STRITO36 will
/      not complain.  This implies that the caller had better make sure
/      that STR is big enough for this number.
/
/   Returns:
/      Always returns STR.
/
/   Error conditions:
/      The size of the set of invalid data is zero, i.e. it can't fail.
/
/   Related functions: str36toi()
/
/   Called by:
/
/   Home: str/strito36.c
*/

/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 18:00 Clean. */
/*: JV 12/16/91 14:28 Add Header. */

#include "int4.h"

static char *alphanums = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

char *strito36 (str, num, len)
   char *str;
   int   len;
   int4  num;
{
   int i=0, j=0, k=0, c;
   /*** The idea for this is taken from C manual, 1st ed., page 60.
   /    It generates the string in reverse order, then reverses and
   /    formats the string, cuz it's easier that way. */
   do {
      str[i++] = alphanums[num % 36];
   } while ((num /= 36) > 0);

   for ( ; i<len; i++) str[i] = '0';  /* Pad out to full length. */
   str[i] = '\0';
   /* Now reverse the stupid thing. */
   for (j=0, k=strlen(str)-1; j < k; j++, k--) {
       c      = str[j];
      str[j] = str[k];
      str[k] = c;
   }
   return (str);
}
