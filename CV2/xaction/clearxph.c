/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CLEAR_XPHEADER.  Initialize all values of a xport file header.
/
/    int n = clear_xpheader (pheader)
/
/    Parameters:
/       XPORTHDR *pheader;
/
/    Purpose:
/       Before using a transaction file header structure, it needs to
/       be initialized.
/
/    How it works:
/       Initialization consists zeroing numeric values and allocating Chix.
/
/   Returns: 1 on success
/            0 if PHEADER not valid.
/
/   Error Conditions:
/      Can't allocate a Chix.
/
/   Related functions:
/
/   Called by: get_xpheader()
/
/   Operating System dependencies: none
/
/   Home: xaction/clearxph.c
/ */

/*: JX  9/14/92 13:46 Create this. */
/*: CR 11/27/92 12:54 Remove potential nested comment. */

#include <stdio.h>
#include "handicap.h"
#include "xaction.h"


FUNCTION clear_xpheader (pheader)
   XPORTHDR *pheader;
{
   ENTRY ("clear_xpheader", "");

   /* Validity check. */
   if (pheader == (XPORTHDR *) NULL) RETURN (0);

   /* First de-allocate previous chixen. */
   if (NOT chxcheck (pheader->origchn, "clear_xpheader")) {
      chxfree (pheader->origchn);
      chxfree (pheader->destchn);
      chxfree (pheader->trueconf);
      chxfree (pheader->confochn);
   }

   pheader->origchn  = chxalloc (L(40), THIN, "clrxport origchn");
   pheader->destchn  = chxalloc (L(40), THIN, "clrxport destchn");
   pheader->trueconf = chxalloc (L(40), THIN, "clrxport trueconf");
   pheader->confochn = chxalloc (L(40), THIN, "clrxport confochn");

   pheader->version[0]= '\0';
   pheader->nayseq[0] = '\0';
   pheader->typenum   = 0;
   pheader->broadcntr[0] = '\0';

   RETURN (1);
}
