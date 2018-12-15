
/*** MAKE_ULIST.   Allocate and initialize a userlist.
/
/    ulist = make_ulist();
/
/    Parameters:
/       Chix     *ulist;       (returned userlist is an array of Chix)
/
/    Purpose:
/       Allocate and initialize a userlist.
/
/    How it works:
/
/    Returns: the allocated array of Chix.
/
/    Error Conditions
/  
/    Side effects:
/
/    Related functions:
/
/    Called by: 
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  ulist/makeulis.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/22/91 22:54 New function. */
/*: CL 12/11/92 15:52 Long sysmem arg. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  Chix * make_ulist()
{
   Chix  *a;
   int    i;
   char  *sysmem();

   ENTRY ("make_ulist", "");

   a = (Chix *) sysmem (L(MAXULIST * sizeof(Chix)), "ulist");
   if (a == (Chix *) NULL)  RETURN (a);

   for (i=0;   i<MAXULIST;   ++i)  a[i] = nullchix;
   RETURN (a);
}
