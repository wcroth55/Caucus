/*** NEW_PRINTRANGE.  Print an Rlist of items or messages.
/
/    new_printrange (out, itemlist, per_line)
/
/    Parameters:
/       int    out;         (write list on this unit code)
/       int    per_line;    (# of 5 digit blocks already taken up on this line)
/       Rlist  itemlist;    (list of selected items)
/
/    Purpose:
/      PRINTRANGE prints on OUT the numbers of the items/messages marked >= 0
/      in the array ITEMLIST.  PRINTRANGE shows consecutive sequences
/      of items as "a-b".
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/
/    Side Effects:
/
/    Related to:
/
/    Called by: 
/
/    Home: conf/printran.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CT  5/28/93 13:47 New function, to replace old printrange(). */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#define   RLNULL   ( (Rlist) NULL )

FUNCTION  new_printrange (out, itemlist, per_line)
   int    out, per_line;
   Rlist  itemlist;
{
   int   seen;
   char  temp[40], range[40];
   Rlist rptr;

   ENTRY ("printrange", "");

   for (seen=0, rptr=itemlist->next;   rptr!=RLNULL;   rptr=rptr->next) {
      seen = 1;
      if (rptr->i0 == rptr->i1)  sprintf (temp, "%5d", rptr->i0);
      else {
         sprintf (range, "%d-%d", rptr->i0, rptr->i1);
         sprintf (temp,  "%10s",  range);
         ++per_line;
      }

      unit_write (out, CQ(temp));
      if (++per_line >= 10)   { unit_write (out, CQ("\n"));   per_line = 0; }
   }

   if (per_line > 0)  unit_write (out, CQ("\n"));
   RETURN (seen);
}
