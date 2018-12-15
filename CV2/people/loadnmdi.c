
/*** LOADNMDIR.   Load name directory for file type N.
/
/    Loads the name directory for file N into NMDIR.  Assumes
/    file N is already locked.  NMDIR must be an empty, newly
/    created namelist. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: DE  4/13/92 12:30 Chixified */
/*: JX  5/27/92 13:08 Fixify. */
/*: JX  6/16/92 14:58 Use *one* str index. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */

#include "caucus.h"

FUNCTION  loadnmdir (n, nmdir)
   int    n;
   struct namlist_t *nmdir;
{
   Chix    s, line, tempchx;
   int4    i, s_index=0;
   int     success;

   ENTRY ("loadnmdir", "");

   if (NOT unit_view (n))  RETURN(0);

   line     = chxalloc (L(160), THIN, "loadnmdi line");
   success  = 0;

   nlclear();
   chxclear(nmdir->str);
   for (nmdir->num = 0;   unit_read (n, line, L(0));   ++nmdir->num) {
      s_index = 0;
      s = line;
      i = chxint4 (s, &s_index);
      while (chxvalue (s, s_index) == ' ')  ++s_index;
      nladd (nmdir, tempchx = CHXSUB(s, s_index), (int)i);
      chxfree ( tempchx );
   }

   unit_close (n);
   DONE(nlerr() ? 0 : 1);

 done:

   chxfree ( line );

   RETURN ( success );
}
