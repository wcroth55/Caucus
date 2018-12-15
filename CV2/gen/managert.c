/*** MANAGER_TEXT.  Interpret and display text set up by the system manager.
/
/    The contents of FILE are displayed exactly as they are, EXCEPT
/    that each '#' is interpreted to mean a 2 second delay.  The '#' is
/    not displayed.  If a '#' is immediately followed by another '#', no
/    delay occurs and only one '#' is displayed. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CR 10/12/88 12:59 Write whole strings, not 1 char at a time. */
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: CR  7/14/89 18:09 Check return code from unitlock, unitview. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JX  5/21/92 13:06 Chixify. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */

#include <stdio.h>
#include "caucus.h"

extern struct flag_template flag;

FUNCTION  manager_text (file)
   Chix   file;
{
   Chix   line, str;
   int4   clen, l;
   int    success;

   ENTRY ("manager_text", "");

   line      = chxalloc (L(160), THIN, "managert line");
   str       = chxalloc (L(160), THIN, "managert str");
   success   = 0;

   if (EMPTYCHX (file))                             SUCCEED;

   if (NOT unit_lock (XIRE, READ, L(0), L(0), L(0), L(0), file))  FAIL;

   if (unit_view (XIRE)) {
      while (unit_read (XIRE, line, L(0))) {

         chxclear(str);
         clen = chxlen (line);
         for (l=0;   l <= clen;  ++l) {
            if      (chxvalue (line, L(l  )) != '#') chxcatsub(str, line, l,   L(1));
            else if (chxvalue (line, L(l+1)) == '#') chxcatsub(str, line, l++, L(1)); 
            else {
               unit_write (XWTX, str);
               syssleep   (2);
               chxclear(str);
	     }
	  }
         chxcatval  (str, THIN, L('\n'));
         unit_write (XWTX, str);
      }
      unit_close (XIRE);
   }
   unit_unlk  (XIRE);
   
   SUCCEED;

 done:

   chxfree ( line );
   chxfree ( str );

   RETURN ( success );
}

