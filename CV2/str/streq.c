/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** STREQ.   Return 1 if string S equals string T; 0 otherwise. */

/*: AA  7/29/88 17:53 Source code master starting point */
/*: CR  9/11/89 17:05 Check for NULL pointers. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  streq (s, t)
   char *s, *t;
{
   ENTRY ("streq.c", "");

   if (s == NULL  ||  t == NULL)  RETURN (0);

   while (*s == *t)  {
      if (NOT *s)  RETURN (1);
      ++s;
      ++t;
   }
   RETURN (0);
}
