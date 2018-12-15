/*** MAKE_XPORTHDR: build an empty transport file header.
/
/    make_xporthdr ()
/
/  Parameters:  none
/
/  Purpose:
/    Allocates memory for and initializes an xport file header.
/
/  How it works:
/
/  Returns:  a pointer to a XPORTHDR or NULL.
/
/  Error Conditions:
/     Returns (XPORTHDR *) NULL if it can't allocate the memory. 
/
/  Side Effects:
/     Allocates memory off the heap.  Caller must free it.
/
/  Related to:
/
/  Called by:  xfp/xp_thruship, xfu/xfu.c
/
/  Home:   xaction/makexpor.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  6/04/92 15:36 Create function. */
/*: CL 12/11/92 15:52 Long sysmem arg. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "xaction.h"

FUNCTION XPORTHDR *make_xporthdr()
{
   XPORTHDR *header;
   char *sysmem();

   ENTRY ("make_xporthdr", "");

   header = (XPORTHDR *) sysmem (L(sizeof (XPORTHDR)), "makexport");
   if (header != (XPORTHDR *) NULL) {
      header->origchn = chxalloc (L(40), THIN, "xporthdr origchn");
      header->destchn = chxalloc (L(40), THIN, "xporthdr destchn");
      header->trueconf = chxalloc (L(40), THIN, "xporthdr trueconf");
      header->confochn = chxalloc (L(40), THIN, "xporthdr confochn");
   }

   RETURN (header);
}
