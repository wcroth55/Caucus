/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** STRCONC.  Concatenate string C onto the end of B, and put it in A. */

/*: AA  7/29/88 17:53 Source code master starting point */
#include "caucus.h"

FUNCTION  strconc (a, b, c)
   char  a[], b[], c[];
{
   int          blen, clen;
   int          strlen();
   register int i;

   ENTRY ("strconc.c", "");
   blen = strlen(b);  clen = strlen(c);
   for (i=clen;  i>=0;    --i)  a[i+blen] = c[i];
   for (i=0;     i< blen; ++i)  a[i]      = b[i];
   RETURN (1);
}
