/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** NLERR.   Return 1 if there was an NL error. */

/*: AA  7/29/88 17:50 Source code master starting point */
#include "caucus.h"

extern char nlerror;

FUNCTION  nlerr()
{
   return ( (int) nlerror);
}
