/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** XFP_PROTECT and XFP_UNPROTECT: prevent two XFP's from running at the
/    same time.
/
/  Returns: XFP_PROTECT returns 1 if this is the only XFP running.
/                    It returns 0 if there is already an XFP running.
/ */

/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/04/91 12:00 Remove lint, entryex.h. */
/*: JV  7/22/91 16:38 Add ENTRY. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: DE  6/05/92 14:37 Chixified */
/*: CR 11/24/92 13:47 Use TLOCK instead of WRITE to test & lock. */

#include <stdio.h>
#include "handicap.h"
#include "unitcode.h"
#include "chixuse.h"

xfp_protect()
{
   if (NOT unit_lock (XSPL, TLOCK, L(0), L(0), L(0), L(0), nullchix)) return (0);
   else return (1);
}

xfp_unprotect()
{
   unit_unlk (XSPL);
   return (1);
}
