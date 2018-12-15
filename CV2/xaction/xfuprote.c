/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** XFU_PROTECT and XFU_UNPROTECT: prevent two XFU's from running at the
/    same time.
/
/  Returns: XFU_PROTECT returns 1 if this is the only XFU running.
/                    It returns 0 if there is already an XFU running.
/ */


/*: JV  7/10/91 16:38 Create function. */
/*: JV  7/22/91 16:39 Add ENTRY. */
/*: JV 11/22/91 12:07 Add lock XSUL & xfu_unprotect. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: DE  6/05/92 14:43 Chixified */
/*: CR 11/24/92 13:47 Use TLOCK instead of WRITE to test & lock. */

#include <stdio.h>
#include "handicap.h"
#include "unitcode.h"
#include "chixuse.h"

xfu_protect()
{
   if (NOT unit_lock (XSUL, TLOCK, L(0), L(0), L(0), L(0), nullchix))  return (0);
   else return (1);
}

xfu_unprotect()
{
   unit_unlk (XSUL);
   return (1);
}
