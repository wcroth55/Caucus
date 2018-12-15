
/*** RL_ALL.   Create an Rlist containing all responses of all items. 
/
/    all = rl_all (items);
/
/    Parameters:
/       Rlist     all;     (returned, newly created Rlist)
/       int       items;   (number of items in conference)
/
/    Purpose:
/       Create an Rlist containing all responses & items. 
/
/    How it works:
/       Extremely simple, just convenient to provide as one function call.
/
/    Returns: new Rlist.
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
/    Home:  api/rlall.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  9/08/93 15:37 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  Rlist rl_all (int items)
{
   Rlist  this, next;

   this = a_mak_rlist ("rl_all this");
   next = a_mak_rlist ("rl_all next");
   this->next = next;
   next->i0   =  1;
   next->i1   =  items;
   next->r0   =  0;
   next->r1   = -1;

   return (this);
}
