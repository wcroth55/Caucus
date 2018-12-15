
/*** IT_PACK.   "Pack" a list of items into a text list.  Reverse of it_parse.
/
/    it_pack  (result, arg);
/   
/    Parameters:
/       Chix    result;  (append resulting value here)
/       Chix    arg;     (function argument)
/
/    Purpose:
/       Evaluate $it_pack(cnum comma triplet_list) function.  Produces
/       a text range of items (e.g., "1-5, 7, 13-20").
/
/    How it works:
/       Ignores items that are not in conference cnum, even if they
/       are in the list.
/
/    Returns: 1 on success (at least one item in CNUM in list)
/             0 otherwise
/
/    Error Conditions:
/  
/    Known bugs:
/       Should it do more checking of deleted/forgotten/etc? items?
/
/    Home:  sweb/itpack.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/03/99 15:10 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  it_pack (Chix result, Chix arg)
{
   Chix   word;
   int4   pos, zero;
   int    count, triplets, tc, ti, tr, seq0, seq1, highest, cnum, psave, comma;
   int    add_trailer;
   char   temp[100];
   char  *trailer;
   char  *mask;
   char  *sysmem();

   ENTRY ("it_pack", "");

   /*** Count number of triplets. */
   word  = chxalloc (L(50), THIN, "it_pack word");
   for (pos=count=0;   chxnextword (word, arg, &pos) >= 0;   ++count) ;
   triplets = (count-1) / 3;
   chxfree (word);

   /*** Get the conf num. */
   pos   = 0;
   cnum  = chxint4 (arg, &pos);
   if (cnum <= 0)  return (0);
   comma = chxint4 (arg, &pos);
   psave = pos;

   /*** Scan to find the highest item # in the triplets. */
   highest = 0;
   for (pos=psave, count=0;   count<triplets;   ++count) {
      tc = chxint4 (arg, &pos);
      ti = chxint4 (arg, &pos);
      tr = chxint4 (arg, &pos);
      if (tc != cnum)    continue;
      if (ti > highest)  highest = ti;
   }
   if (highest == 0)                             return (0);

   /*** Allocate the mask and clear it.  Force a 0 cell at the end. */
   ++highest;
   if  ( (mask = sysmem (highest+1, "itpack")) == NULL)    return (0);
   for (count=0;   count <= highest;   ++count)  mask[count] = 0;

   /*** Scan again to fill in the mask of selected items. */
   for (pos=psave, count=0;   count<triplets;   ++count) {
      tc = chxint4 (arg, &pos);
      ti = chxint4 (arg, &pos);
      tr = chxint4 (arg, &pos);
      if (tc != cnum)  continue;
      mask[ti] = 1;
   }

   /*** Scan the mask, and output sequences ("5-8") as found. */
   seq0 = seq1 = 0;
   for (add_trailer=count=0;   count <= highest;   ++count) {
      if (mask[count]) {
         if (NOT seq0)  seq0 = count;
         seq1 = count;
      }
      else if (seq0) {
         trailer = (add_trailer ? (comma ? ", " : " ") : "");
         if (seq1 > seq0)  sprintf (temp, "%s%d-%d", trailer, seq0, seq1);
         else              sprintf (temp, "%s%d",    trailer, seq0);
         chxcat  (result, CQ(temp));
         seq0 = seq1 = 0;
         add_trailer = 1;
      }
   }

   sysfree (mask);

   RETURN (1);
}
