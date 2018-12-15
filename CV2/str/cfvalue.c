/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** CF_VALUE.  Return equivalent value of digit D.
/
/    I.E., if D is '5', CF_VALUE returns the integer 5. */

/*: AA  7/29/88 17:52 Source code master starting point */
/*: JV  8/01/88 15:08 Added AOS code.*/
/*: CR 11/09/88 15:18 Remove VM, add CX, CA, CVM. */
/*: CW  6/07/89 16:37 Add PX system type. */
/*: CR  9/26/90 10:13 Add PS system type. */

#include "caucus.h"
#include "systype.h"

FUNCTION  cf_value (d)
   int    d;
{

#if UNIX | NUT40 | WNT40
   return ( d - '0');
#endif

}
