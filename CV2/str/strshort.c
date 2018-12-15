/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** STRSHORT.  Scans NUMBER short ints into ARRAY from string STR.
/
/    If there are fewer than NUMBER ints in STR, STRSHORT fills
/    the rest with 0's.   STRSHORT always returns 1. */

/*: AM  2/07/89 10:47 Created for Jcaucus*/
/*: CW  6/16/89 13:15 Remove control characters from header! */
/*: CR  8/31/89 14:16 Expanded header comments. */
/*: CR  9/26/90 10:33 Make Lint happy if JAPAN is OFF. */

#include <ctype.h>
#include "caucus.h"
#include "systype.h"

extern struct flag_template flag;

FUNCTION  strshort (array, str, number)
   short array[];
   char  *str;
   int   number;
{
   int   i, negative;
   char  prevchar;
#if JAPAN
   char  type, prevtype;
#endif

   ENTRY ("strshort", "");

#if JAPAN
   if (flag.japan)
      for (i = 0;  i < number; i++) {
         prevtype = prevchar = '\0';
         while (*str && (NOT cf_single(*str, &type) || NOT cf_digit(*str))) {
            prevtype = type;
            prevchar = *str++;
         }
         negative = (prevtype == SBC_BYTE && prevchar == '-');
         array[i] = 0;
         while (*str && cf_digit(*str))
            array[i] = array[i] * 10 + cf_value(*str++);
         if (negative)
            array[i] = -array[i];
      }
   else
#endif

      for (i = 0;  i < number; i++) {
         prevchar = '\0';
         while (*str && NOT cf_digit(*str))
            prevchar = *str++;
         negative = (prevchar == '-');
         array[i] = 0;
         while (*str && cf_digit(*str))
               array[i] = array[i] * 10 + cf_value(*str++);
         if (negative)
            array[i] = -array[i];
      }
   RETURN (1);
}
