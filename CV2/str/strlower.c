/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** STRLOWER.  Convert string S to all lower case letters. */

void strlower (s)
   char  *s;
{

   for (;   *s;   ++s)    if (*s>='A'  &&  *s<='Z')    *s += ('a' - 'A');

   return;
}
