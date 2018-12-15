
/*** strunescape.   Undo javascript escape() function on a string.
/
/    strunescape (str);
/
/    Parameters:
/       char   *str;
/
/    Purpose:
/       Javascript's escape() function encodes various special
/       characters as the hexidecimal encoding "%xx".  Strunescape()
/       "unescapes" such an encoded string.
/
/    Home:  str/strunescape.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 01/31/05 New function. */

#include <stdio.h>

int strunescape (char *str) {
   unsigned char *s, *t;
   char           hex[3] = {0, 0, 0};

   for (s=t=str;   *s;   ++s) {
      if (*s != '%'  ||  !(*(s+1))  || !(*(s+2)) )  *t++ = *s;
      else {
         hex[0] = *(s+1);
         hex[1] = *(s+2);
         *t++   =  (strtol (hex, NULL, 16) & 0xFF);
         s += 2;
      }
   }
   *t = '\0';
   return (1);
}
