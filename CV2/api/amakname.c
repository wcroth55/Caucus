/*** A_MAK_NAMEX.   Make (allocate) a Namex object.
/
/    namex = a_mak_namex (name);
/
/    Parameters:
/       Namex   namex;   (returned/created object)
/       char   *name;    (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize a Namex object.
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
/    Related functions:  a_fre_namex()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amakname.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/15/93 21:19 New function. */
/*: CP  8/01/93  9:37 Add 'name' argument, debugging stuff. */
/*: CP  8/02/93 12:42 Add a_mak_rlist() arg. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  Namex  a_mak_namex (char *name)
{
   Namex  namex;

   namex = (Namex) sysmem (sizeof (struct Namex_t), "namex");
   if (namex == (Namex) NULL)  return (namex);

   namex->tag      = T_NAMEX;
   A_SET_NAME (namex->dname, name);
   namex->cnum     =  0;
   namex->owner    = chxalloc (L(20), THIN, "a_mak_su owner");
   namex->namepart = chxalloc (L(80), THIN, "a_mak_su namepart");
   namex->rlist    = a_mak_rlist("amakname namex");
   namex->ready    = 0;

   if (api_debug) a_debug (A_ALLOC, (Ageneric) namex);

   return (namex);
}
