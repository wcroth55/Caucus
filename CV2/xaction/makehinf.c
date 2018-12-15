/*** MAKE_HINFO.   Allocate and initialize a HOSTINFO structure..
/
/    i = make_hinfo();
/
/    Parameters:
/       Hinfoptr i;     (returned HOSTINFO (struct hostinfo_t))
/
/    Purpose:
/       Create a new, empty hostinfo-T structure.
/
/    How it works:
/
/    Returns: a new hostinfo_t struct
/             NULL on error
/
/    Error Conditions:
/       Insufficient memory, or the heap is screwed up.
/  
/    Side effects:
/
/    Related functions: make_host()  free_host()
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  xaction/makehinf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  7/24/92 15:46 New function. */
/*: CL 12/11/92 15:52 Long sysmem arg. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "xfp.h"

FUNCTION   Hinfoptr  make_hinfo ()
{
   Hinfoptr i;
   char    *sysmem();

   ENTRY ("make_hinfo", "");

   i = (Hinfoptr) sysmem (L(sizeof (struct hostinfo_t)), "make_hinfo");
   if (i == (Hinfoptr) NULL)  RETURN (i);

   i->mgrid   = chxalloc (L(80), THIN, "make_host mgrid");

   RETURN (i);
}
