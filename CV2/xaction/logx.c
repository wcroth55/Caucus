/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/* LOGX: log a xaction that just couldn't be dealt with.
/        Usually this means there's a software problem. */

/*: JV  6/27/91 10:41 Created this. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV 10/31/91 09:29 Only printxact() if debug. */

#include <stdio.h>
#include "caucus.h"
#include "handicap.h"
#include "xaction.h"

extern int debug;

FUNCTION logx (str, xptr)
char *str;
XACTIONPTR xptr;
{
   if (debug) {
      printf ("%s\n", str);
      printxact (xptr);
   }
}
