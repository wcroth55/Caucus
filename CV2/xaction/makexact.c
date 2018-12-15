/*** MAKE_XACTION: build an empty transaction.
/
/    pxact = make_xaction ()
/
/  Parameters:
/    Xactionptr pxact;
/
/  Purpose:
/    Allocates memory for and initializes a xaction.
/
/  How it works:
/
/  Returns:  a pointer to a Xactionptr or NULL.
/
/  Error Conditions:
/     Returns (Xactionptr) NULL if it can't allocate the memory. 
/
/  Side Effects:
/     Allocates memory off the heap.  Caller must free it with free_xaction.
/
/  Related to: clear_xaction(),  free_xaction()
/
/  Called by:  xfp/xp_thruship, xfu/xfu.c
/
/  Home:   xaction/makexact.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  9/01/92 10:36 Create function. */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: JV  2/24/93 13:32 Add File Attachment. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "attach.h"
#include "xaction.h"

FUNCTION Xactionptr make_xaction()
{
   Xactionptr xact;
   char *sysmem();
   int i;

   ENTRY ("make_xaction", "");

   xact = (Xactionptr) sysmem (L(sizeof (XACTION)), "makexact");
   if (xact != (Xactionptr) NULL) {
      xact->conftname = chxalloc (L(32), THIN, "makexact conftname");
      xact->confochn  = chxalloc (L(32), THIN, "makexact confochn");
      xact->uid       = chxalloc (L(32), THIN, "makexact uid");
      xact->uname     = chxalloc (L(32), THIN, "makexact uname");
      xact->unewname  = chxalloc (L(32), THIN, "makexact unewname");
      xact->phone     = chxalloc (L(32), THIN, "makexact phone");
      xact->itemchn   = chxalloc (L(32), THIN, "makexact itemchn");
      xact->respchn   = chxalloc (L(32), THIN, "makexact respchn");
      xact->rechost   = chxalloc (L(32), THIN, "makexact rechost");
      xact->recid     = chxalloc (L(32), THIN, "makexact recid");
      xact->recname   = chxalloc (L(32), THIN, "makexact recname");
      xact->title     = chxalloc (L(32), THIN, "makexact title");
      xact->date      = chxalloc (L(32), THIN, "makexact date");
      xact->daterecvd = chxalloc (L(32), THIN, "makexact daterecvd");
      for (i=0; i<TEXTFIELDS; i++)
         xact->data[i] = chxalloc (L(256), THIN, "makexact data");

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
      xact->attach = make_attach ();
   }

   RETURN (xact);
}
