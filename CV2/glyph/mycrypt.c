/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** MYCRYPT.  Encrypt a short string into a int4 int. */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: JX  5/19/92 13:00 Chixify. */
/*: CL 12/10/92 17:06 Long ascofchx args. */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: CR 12/21/92 15:33 Fix off-by-one error in CLEN. */
/*: CR  1/15/95 14:51 Allocate str off stack, fix off-by-one jcaucus err. */

#include <stdio.h>
#include "caucus.h"

#define  MAXSTR    100

static int4 power[11] = {2, 128, 1, 8, 16, 2, 128, 4, 64, 8, 1};

FUNCTION  int4 mycrypt (c)
   Chix  c;
{
   char *s;
   char  str[MAXSTR];
   int4  sum, clen;

   ENTRY ("mycrypt", "");

   if (EMPTYCHX (c))  RETURN (L(0));

   ascofchx (str, c, L(0), L(MAXSTR));
   s = str;

   for (sum = *s++;   *s;   ++s) {
      sum = sum + *s * power[ (*(s-1) % 11) ];
   }

   RETURN (sum);
}
