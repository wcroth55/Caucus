
/*** RL_OF_SELECT.   Convert a "select" item list into an Rlist. 
/
/    rl = rl_of_select (select, maxitems);
/
/    Parameters:
/       Rlist     rl;        (returned new Rlist)
/       short     select[];  (itemlist to be converted)
/       int       maxitems;  (number of items in SELECT)
/
/    Purpose:
/       Take a "select" item list, and convert it into an Rlist.
/
/    How it works:
/       SELECT[I] is   -1 if the item is not selected
/                       0 if the entire item selected
/                     n>0 if responses N thorugh the end are selected
/
/       Rl_of_select() scans through SELECT and assembles "rectangular
/       blocks" of selected item/response ranges, and adds them as
/       nodes to a newly allocated Rlist.  It returns that Rlist.
/
/    Returns: new Rlist
/
/    Error Conditions:
/
/    Side effects: 
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/rlofsele.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  5/18/93 15:31 New function. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  Rlist rl_of_select (short *select, int maxitems)
{
   Rlist  rthis, tail;
   int    item, prev;

   /*** PREV > 0 means we are still trying to assemble a rectangular
   /    "block" of responses, and PREV contains the number of the
   /    first item in that block. */

   rthis = tail = a_mak_rlist("rlofsele rthis");
   for (prev=0, item=1;   item<=maxitems;   ++item) {

      /*** When we reach a non-selected item, stop assembling a block. */
      if (select[item] < 0)  prev = 0;

      /*** First item in a new block.  Allocate an Rlist node and 
      /    set up the block to be this item. */
      else if (prev==0) {
         tail->next = a_mak_rlist("rlofsele next");
         tail     = tail->next;
         tail->i0 = tail->i1 = item;
         tail->r0 = select[item];
         tail->r1 = -1;
         prev = item;
      }

      /*** If this item is the same "size" as the block we're assembling,
      /    add it on. */
      else if (select[item] == tail->r0)  tail->i1 = item;

      /*** Otherwise, back up one item, and prepare to start a new block. */
      else { prev = 0;   --item; }
   }

   return (rthis);
}
