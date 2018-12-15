/*** FREE_XPORTHDR: free the memory previously allocated for an empty
/    transport file header.
/
/    free_xporthdr (header)
/
/  Parameters:  XPORTHDR *header;
/
/  Purpose:
/    Frees memory associated with a xport file header.
/
/  How it works:
/
/  Returns:  1 on success, 0 if header doesn't point to valid memory.
/
/  Error Conditions:
/
/  Side Effects:
/
/  Related to:
/
/  Called by:  xfp/xp_thruship, xfu/xfu.c
/
/  Home:   xaction/freexpor.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  6/04/92 15:36 Create function. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "xaction.h"

FUNCTION free_xporthdr (header)
XPORTHDR *header;
{
   ENTRY ("free_xporthdr", "");

   if (header != (XPORTHDR *) NULL) {
      chxfree (header->origchn);
      chxfree (header->destchn);
      chxfree (header->trueconf);
      chxfree (header->confochn);
      sysfree ((char *) header);
      RETURN (1);
   } else RETURN (0);

}
