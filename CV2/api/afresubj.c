/*** A_FRE_SUBJ.   Free (deallocate) a Subj object.
/
/    a_fre_subj (subj);
/
/    Parameters:
/       Subj     subj;   (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by a_mak_subj().
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if subj is null or not an Subj.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_subj()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afresubj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/12/93 08:22 New function. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  a_fre_subj (Subj subj)
{

   if (subj == (Subj) NULL)  return (0);
   if (subj->tag != T_SUBJ)  return (0);

   if (api_debug) a_debug (A_FREE, (Ageneric) subj);
   subj->tag = 0;

   chxfree (subj->owner);
   chxfree (subj->namepart);
   nlfree  (subj->names);

   sysfree ( (char *) subj);

   return (1);
}
