/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** PARTNUM.  Get partfile number from response, linenumber.
/
/    PARTNUM is handed a partfile directory (PDIR), and a response
/    and line number (RESP, LNUM).  It returns the index into PDIR
/    of the partfile that contains that response/linenumber.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CR 12/31/91 12:40 Expand test into separate if's for debugging. */

#include "caucus.h"

FUNCTION  partnum (pdir, resp, lnum)
   struct pdir_t  *pdir;
   int    resp, lnum;
{
   int    p;

   ENTRY ("partnum", "");

   if (pdir->parts == 0) {
      pdir->resp[0] = resp;
      pdir->lnum[0] = lnum;
      pdir->parts   = 1;
   }

   pdir->resp [pdir->parts] = 16000;

   for (p=1;   p<pdir->parts;   ++p) {
      if (resp <  pdir->resp[p])                              break;
      if (resp == pdir->resp[p]  &&  lnum < pdir->lnum[p])    break;
   }

   RETURN (p-1);
}
