/*** CLEAR_XACTION: initialize all of the fields of a transaction.
/
/    ok = clear_xaction (xact)
/
/  Parameters:
/    int        ok;         // Return value
/    Xactionptr xact;       // Pointer to a xaction which has been initialized
/
/  Purpose:
/    Initializes a xaction.
/
/  How it works:
/
/  Returns:  1 on success, 0 on failure
/
/  Error Conditions:
/     Returns 0 if this isn't a valid Xactionptr
/
/  Side Effects:
/
/  Related to: make_xaction(), free_xaction()
/
/  Called by:  
/
/  Home:   xaction/clearxac.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  9/30/92 10:36 Create function. */
/*: JV  2/24/93 13:31 Add File Attachment */
/*: CR 10/27/93 18:24 Change test from "xact != NULL" to "xact == NULL". */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "xaction.h"

FUNCTION  clear_xaction (xact)
   Xactionptr xact;
{
   int i;

   ENTRY ("clear_xaction", "");

   if (xact == (Xactionptr) NULL) RETURN (0);

   chxclear (xact->conftname);
   chxclear (xact->confochn);
   chxclear (xact->uid);
   chxclear (xact->uname);
   chxclear (xact->unewname);
   chxclear (xact->phone);
   chxclear (xact->itemchn);
   chxclear (xact->respchn);
   chxclear (xact->rechost);
   chxclear (xact->recid);
   chxclear (xact->recname);
   chxclear (xact->title);
   chxclear (xact->date);
   chxclear (xact->daterecvd);
   clear_attach (xact->attach);

   for (i=0; i<TEXTFIELDS; i++) chxclear (xact->data[i]);

   xact->mtype[0] = '\0';   xact->mtypenum = -1;
   xact->rrlength = -1;
   xact->type[0]    = '\0'; xact->typenum=0;
   xact->action[0]  = '\0'; xact->actnum=0;
   xact->subtype[0] = '\0'; xact->subnum=0;
   xact->conflocnum=0L;
   xact->itemid=0L;
   xact->respid=0L;
   xact->m_oparts=0;
   xact->mailctrl=0;
   xact->transit=0;
   for (i=0; i<TEXTFIELDS; i++) xact->datasize[i]=0L;

   RETURN (1);
}
