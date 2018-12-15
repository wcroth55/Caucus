/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** PDIRPRINT.   Displays PDIR on unit U.   Debugging tool only. */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: JV 10/31/91 15:14 Declare sprintf */
/*: JV 12/16/91 16:37 Change pdir.lnum to int4. */
/*: DE  4/01/92 16:44 Chixified */
/*: JX  5/27/92 15:27 Ascquick strito36(arg1), chxquick strito36. */

#include "caucus.h"

extern union null_t      null;

FUNCTION  pdirprint (u, pdir)
   int    u;
   struct pdir_t *pdir;
{
   Chix   buf, std; 
   char  *strito36();
   int    i;

   buf        = chxalloc (L(200), THIN, "pdirprin buf");
   std        = chxalloc (L(12),  THIN, "pdirprin std");

   chxformat (buf, CQ("PDIR: type=%d, parts=%d\n"), L(pdir->type),
              L(pdir->parts), null.chx, null.chx);
   unit_write (u, buf);
   for (i=0;   i<pdir->parts;   ++i) {
      chxclear ( buf );
      chxformat (buf, chxquick ("%3d:  %4d %s\n", 3), L(i), L(pdir->resp[i]),
                 chxquick (strito36 (ascquick (std), pdir->lnum[i], 4), 4),
                 null.chx);
      unit_write (u, buf);
   }

   chxfree ( buf );
   chxfree ( std );

}
