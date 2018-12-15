/*** A_MAK_SUBJ.   Make (allocate) an Subj object.
/
/    subj = a_mak_subj (name);
/
/    Parameters:
/       Subj   subj;   (returned/created object)
/       char  *name;   (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize a Subj object.
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
/    Related functions:  a_fre_subj()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amaksubj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/12/93 08:22 New function. */
/*: CP  8/01/93  9:37 Add 'name' argument, debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  Subj  a_mak_subj (char *name)
{
   Subj     subj;
   Namelist nlnode();

   subj = (Subj) sysmem (sizeof (struct Subj_t), "Subj_t");
   if (subj == (Subj) NULL)  return (subj);

   subj->tag      = T_SUBJ;
   A_SET_NAME (subj->dname, name);
   subj->cnum     =  0;
   subj->owner    = chxalloc (L(20), THIN, "a_mak_su owner");
   subj->namepart = chxalloc (L(80), THIN, "a_mak_su namepart");
   subj->names    = nlnode(0);
   subj->ready    = 0;

   if (api_debug) a_debug (A_ALLOC, (Ageneric) subj);

   return (subj);
}
