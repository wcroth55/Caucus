
/*** ADD_TO_RLIST.   Add an item with a response range onto the end of an Rlist.
/
/    newptr = add_to_rlist (oldptr, item, r0, r1);
/
/    Parameters:
/       Rlist     newptr;     (returned node in Rlist)
/       Rlist     oldptr;     (original Rlist)
/       int       item;       (item number)
/       int       r0;         (first response in range)
/       int       r1;         (last  response in range)
/
/    Purpose:
/       Add an item with a response range onto the end of an Rlist.
/
/    How it works:
/       OLDPTR is a pointer to a node in an Rlist.  Add_to_rlist() tries
/       to add the response range ITEM:R0-R1 to node OLDPTR.  If that fails,
/       it keeps stepping down the Rlist until it succeeds.  If no node
/       succeeds, it creates a new node, sets the node to cover ITEM:R0-R1,
/       and tacks the new node onto the end of the Rlist.
/
/       The function is often called as "rptr = add_to_rlist (rptr, i, r0, r1);"
/
/    Returns: pointer to node that (now) contains ITEM:R0-R1.
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
/    Home:  api/addtorl.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CT  5/28/93 14:10 New function. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

#define  RLNULL     ((Rlist) NULL)

FUNCTION  Rlist add_to_rlist (Rlist oldptr, int item, int r0, int r1)
{
   Rlist  newptr, lastptr;

   for (newptr=oldptr;   newptr!=RLNULL;   newptr=newptr->next) {
      lastptr = newptr;
      if (item == newptr->i1+1  &&  r0 == newptr->r0  &&  r1 == newptr->r1) {
         newptr->i1 = item;
         return (newptr);
      }
   }

   lastptr->next = newptr = a_mak_rlist("addtorl rlist");
   newptr->i0 = newptr->i1 = item;
   newptr->r0 = r0;
   newptr->r1 = r1;
   return (newptr);
}
