/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** CF_UPPER.   Is character C a upper-case alphabetic character? */

/*: AA  7/29/88 17:52 Source code master starting point */
/*: JV  8/01/88 15:06 Added AOS code.*/
/*: CR 11/09/88 15:18 Remove VM, add CX, CA, CVM. */
/*: CW  6/07/89 16:37 Add PX system type. */
/*: CR  9/26/90 10:13 Add PS system type. */

#include "caucus.h"
#include "systype.h"

FUNCTION  cf_upper (c)
   int    c;
{

#if UNIX | NUT40 | WNT40
   return (c >= 'A'   &&   c <= 'Z');
#endif
}
