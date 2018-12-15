/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** STRALTER.  Alter 1st occurence of OLD in SSS to NEW. */

/*: AA  7/29/88 17:53 Source code master starting point */
#include "caucus.h"

FUNCTION  stralter (sss, old, new)
   char  sss[], old[], new[];
{
   int   ssslen, oldlen, newlen, pos, delta;
   register int i;
   int   strlen(), strindex();

   ENTRY ("stralter.c", "");
   ssslen = strlen (sss);   oldlen = strlen (old);   newlen = strlen (new);
   if (ssslen==0 || oldlen==0)         RETURN (0);

   if ((pos = strindex (sss, old)) < 0)  RETURN (0);

   delta = newlen - oldlen;
   if (delta > 0) {
      for (i=ssslen;  i>= pos+oldlen;  --i)  sss[i+delta] = sss[i]; }
   if (delta < 0) {
      for (i=pos+oldlen;  i<=ssslen;   ++i)  sss[i+delta] = sss[i]; }

   for (i=0;  i<newlen;  ++i)  sss[i+pos] = new[i];

   RETURN (1);
}
