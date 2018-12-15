/*** MAKE_HOST.   Allocate and initialize a HOST.
/
/    h = make_host();
/
/    Parameters:
/       HOSTPTR  h;     (returned HOST (struct host_t))
/
/    Purpose:
/       Create a new, empty host structure.
/
/    How it works:
/
/    Returns: a new host_t struct
/             NULL on error
/
/    Error Conditions:
/       Insufficient memory, or the heap is screwed up.
/  
/    Side effects:
/
/    Related functions: free_host()
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  xaction/makehost.c
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

FUNCTION   HOSTPTR  make_host ()
{
   HOSTPTR h;
   char   *sysmem();

   ENTRY ("make_host", "");

   h = (HOSTPTR) sysmem (L(sizeof (struct host_t)), "make_host");
   if (h == NULL)  RETURN (h);

   h->chn     = chxalloc (L(80), THIN, "make_host chn");
   h->lhname  = chxalloc (L(40), THIN, "make_user lhname");

   RETURN (h);
}
