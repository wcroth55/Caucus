
/*** SYSTIME.  Returns absolute system time (in some constant units,
/    presumably seconds since some fixed date.) as a LONG int. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include <time.h>
#include "caucus.h"
#include "systype.h"

FUNCTION  int4 systime()
{

#if UNIX | NUT40
   ENTRY ("systime", "");
   RETURN ((int4) time (NULL) );
#endif


#if WNT40
   return (time (NULL));
#endif

}
