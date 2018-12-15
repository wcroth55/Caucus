/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** STRSUB.  Take the substring of STR, starting at position START,
/    LEN characters int4, and place it in RESULT.  The substring will
/    stop at the end of STR, regardless of the value of LEN. */

/*: AA  7/29/88 17:53 Source code master starting point */
#include "caucus.h"

FUNCTION  strsub (result, str, start, len)
   char  result[], str[];
   int   start, len;
{
   int          last, strlen(), min(), max();
   register int i,k;

   ENTRY ("strsub.c", "");
   start = max (start, 0);
   last  = min (start + len -1, strlen(str)-1);
   for (i=0, k=start;  k<=last;  i++, k++)   result[i] = str[k];
   result[i] = '\0';
   RETURN (1);
}
