/*** PRINTRANGE.  Print a list of items or messages.
/
/  printrange (out, itemlist, highest, per_line)
/
/  Parameters:
/     int    out, highest, per_line;
/     short *itemlist;    (list of selected items)
/
/  Purpose:
/    PRINTRANGE prints on OUT the numbers of the items/messages marked >= 0
/    in the array ITEMLIST.  PRINTRANGE shows consecutive sequences
/    of items as "a-b".   PRINTRANGE only considers items 1 through
/    HIGHEST. 
/
/  How it works:
/
/  Returns:
/
/  Error Conditions:
/
/  Side Effects:
/
/  Related to:
/
/  Called by: 
/
/  Home: conf/printran.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: CR 12/30/88 16:13 Replace "   "+n with blanks+n. */
/*: CR  4/21/88 16:50 Remove itemlist[MAXITEMS] initialization. */
/*: JV 10/31/91 15:08 Declare sprintf */
/*: JX  3/19/92 21:00 Chixify, add header. */
/*: DE  5/26/92 12:50 Fix inconsistant args */
/*: JX  6/19/92 17:40 Fixify. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */
/*: CN  5/15/93 15:22 Itemlist is now an array of short's. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"

FUNCTION  printrange (out, itemlist, highest, per_line)
   int    out, highest, per_line;
   short *itemlist;
{
   int   i, start;
   char  noitems;
   Chix  str, blanks, field;

   ENTRY ("printrange", "");

   str      = chxalloc (L(20), THIN, "printr str");
   field    = chxalloc (L(20), THIN, "field");
   blanks   = chxalloc (L(20), THIN, "blanks");

   chxofascii (blanks, "          ");
   itemlist[highest+1] = -1;
   start               =  0;

   for (noitems=1, i=1;   i<=highest+1;   ++i) {
      if (itemlist[i] >= 0) {
         noitems = 0;
         if (NOT start)  start = i;
      }
      else  if (start) {
         chxclear (str);
         if (start == i-1)
            chxformat (str, CQ("%5d"), L(start), L(0), nullchix, nullchix);
         else {
            /* Build line that's right-adjusted in {chxlen(blanks)} columns. */
            chxclear  (field);
            chxformat (field, CQ("%d-%d"), L(start), L(i-1), nullchix, 
                       nullchix); 
            chxcatsub (str, blanks, L(0), chxlen(blanks)-chxlen(field));
            chxconc   (str, str, field);
            ++per_line;
         }

         unit_write (out, str);
         if (++per_line >= 10)   { unit_write (out, CQ("\n"));   per_line = 0; }

         start = 0;
      }
   }

   if (per_line > 0)  unit_write (out, CQ("\n"));
   RETURN (NOT noitems);
}
