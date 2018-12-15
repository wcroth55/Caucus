/*** FREE_XACTION: free the memory associated with a  transaction.
/
/    free_xaction (xact)
/
/  Parameters:
/    Xactionptr pxact;
/
/  Purpose:
/    Deallocates memory for a xaction.
/
/  How it works:
/
/  Returns:  1.
/
/  Error Conditions:
/
/  Side Effects:
/
/  Related to: make_xaction()
/
/  Called by:  xfp/xp_thruship, xfu/xfu.c
/
/  Home:   xaction/freexact.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  9/01/92 12:40 Create function. */
/*: JX 10/09/92 18:02 Cast arg to sysfree. */
/*: JV  2/24/93 13:33 Add File Attachments. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "xaction.h"

FUNCTION  free_xaction (xact)
   Xactionptr xact;
{
   int i;

   ENTRY ("free_xaction", "");

   if (xact != (Xactionptr) NULL) {
      chxfree (xact->conftname);
      chxfree (xact->confochn);
      chxfree (xact->uid);
      chxfree (xact->uname);
      chxfree (xact->unewname);
      chxfree (xact->phone);
      chxfree (xact->itemchn);
      chxfree (xact->respchn);
      chxfree (xact->rechost);
      chxfree (xact->recid);
      chxfree (xact->recname);
      chxfree (xact->title);
      chxfree (xact->date);
      chxfree (xact->daterecvd);
      for (i=0; i<TEXTFIELDS; i++)
         chxfree (xact->data[i]);
      free_attach (&xact->attach);
   }
   sysfree ((char *) xact);

   RETURN (1);
}
