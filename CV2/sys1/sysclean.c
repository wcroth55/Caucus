
/*** SYSCLEAN.   Remove all non-printable characters from a string S. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: CR 11/02/88 16:23 Add CX to SX, remove VMU, change VMR -> CVM. */
/*: JV 12/07/88 10:37 Added CA code. */
/*: CW  5/08/89 15:17 Add PX system code. */
/*: JV 10/04/89 17:53 Stripped out AOS non-server code. */
/*: CR  4/12/90 16:51 Use char_map & sysio for filtering. */
/*: CR  9/20/90 17:03 Add PS system type. */
/*: CJ 11/22/90 21:54 Change use of sysio members. */
/*: CX  6/05/92 16:30 Make sysclean() a no-op for now. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "sys.h"

FUNCTION  sysclean (s)
   Chix   s;
{
}
