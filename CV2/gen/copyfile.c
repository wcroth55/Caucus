
/*** COPYFILE.   Copy file A to file B.
/
/    Copy the contents of file A (unit AU, item/resp/lnum A1, A2, A3,
/    word AS) to file B (etc).  Return 1 on success, 0 otherwise.
/  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/26/90 22:37 New function. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: DE  3/31/92 12:23 Chixified */
/*: DE  6/02/92 11:15 Added maxlen arg to unit_read */

#include "caucus.h"

extern short confnum;

FUNCTION  copyfile (au,a1,a2,a3,as, bu,b1,b2,b3,bs)
   int    au, a1, a2, a3, bu, b1, b2, b3;
   Chix   as, bs;
{
   Chix   line;
   int    success;

   ENTRY ("copyfile", "");

   if (NOT unit_lock (au, READ, L(confnum), L(a1), L(a2), L(a3), as))
                                                  RETURN (0);
   if (NOT unit_view (au))  { unit_unlk (au);     RETURN (0); }

   line     = chxalloc (L(250), THIN,  "copyfile line");
   success  = 0;

   if (unit_lock (bu, WRITE, L(confnum), L(b1), L(b2), L(b3), bs)) {
      if (unit_make (bu)) {
         while (unit_read (au, line, L(0)))  {
            unit_write (bu, line);
            unit_write (bu, CQ("\n"));
         }
         success = 1;
         unit_close (bu);
      }
      unit_unlk (bu);
   }

   unit_close (au);
   unit_unlk  (au);

   chxfree ( line );

   RETURN (success);
}
