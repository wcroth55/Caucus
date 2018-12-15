/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*: AA  7/29/88 17:52 Source code master starting point */
#include "caucus.h"

FUNCTION   max (a, b)
   int  a, b;
{
   ENTRY ("max.c", "");
   RETURN ( a>b ? a : b);
}
