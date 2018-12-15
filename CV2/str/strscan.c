
/*** STRSCAN.   Scan a string for an integer.
/
/    STRSCAN is handed S, a pointer to a pointer to a character string.
/    It scans through the string by incrementing *S, until it runs
/    out of digits.  The int4 integer equivalent to the digits it scanned
/    is RETURNed as the result.   STRSCAN will skip leading blanks. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:53 Source code master starting point */
/*: CR 11/03/88 14:10 Change name on copyright. */
/*: JV  2/07/89 15:41 Added check for null pointer. */

#include "caucus.h"

FUNCTION  int4  strscan (s)
   char **s;
{
   int4  result;
   char *p;

   ENTRY ("strscan", "");

   /*** Skip over leading blanks.  If the first non-blank char is NOT
   /    a digit, restore S to its original value.  (The caller will know
   /    that this means ERROR. */
   if (NOT *s) RETURN (-1);     /* Pointed to 0. Bad! */
   p = *s;
   while (**s == ' ')     ++*s;
   if (NOT cf_digit (**s))  {  *s = p;   RETURN (0); }

   result = 0;
   while  (cf_digit (**s))  result = L(10) * result + cf_value (*(*s)++);

   RETURN (result);
}
