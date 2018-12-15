
/*** SYSBRAND.   Check BRANDNEW routine at every AND NOW?
/
/    SYSBRAND controls how often BRANDNEW is called before the
/    AND NOW? prompt.  On some systems it is called at every
/    prompt.  On others we may only want to do it every so often.
/    On single user systems, we don't need to do it at all.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: CR 11/02/88 16:05 Add SRV to UNIX code. */
/*: CW  5/08/89 15:17 Add PX system code. */
/*: JV 10/04/89 17:36 Stripped out AOS non-server stuff. */
/*: CR  9/20/90 17:03 Add PS system type. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

FUNCTION  sysbrand()
{
   ENTRY ("sysbrand", "");

#if UNIX | NUT40 | WNT40
   RETURN (1);
#endif
}
