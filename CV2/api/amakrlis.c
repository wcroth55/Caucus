/*** A_MAK_RLIST.   Make (allocate) an Rlist object.
/
/    rlist = a_mak_rlist (name);
/
/    Parameters:
/       Rlist   rlist;   (returned/created object)
/       char   *name;    (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize a Rlist object.
/
/    How it works:
/
/    Returns: newly created object
/             NULL on error
/
/    Error Conditions: insufficient heap memory, or heap catastrophe.
/  
/    Side effects: 
/
/    Related functions:  a_fre_rlist()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amakrlis.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/15/93 23:36 New function. */
/*: CT  5/28/93 14:14 Initialize item to -1. */
/*: CP  6/24/93 15:50 Initialize item to -9999. */
/*: CP  8/01/93  9:37 Add 'name' argument, debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  Rlist  a_mak_rlist (char *name)
{
   Rlist  rlist;

   rlist = (Rlist) sysmem (sizeof (struct Rlist_t), "Rlist_t");
   if (rlist == (Rlist) NULL)  return (rlist);

   rlist->tag  = T_RLIST;
   rlist->i0   = rlist->i1 = -9999;
   rlist->r0   =  0;
   rlist->r1   = -1;
   rlist->next = (Rlist) NULL;
   A_SET_NAME (rlist->dname, name);

   if (api_debug) a_debug (A_ALLOC, (Ageneric) rlist);

   return (rlist);
}
