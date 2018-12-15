
/*** CHXFREE.  Free a chix allocated by chxalloc().
/
/    chxfree (x);
/
/    Parameters:
/       Chix  x;
/
/    Purpose:
/       Deallocate a chix allocated by chxalloc(), and return the
/       memory back to the heap.
/
/    How it works:
/       Chxfree() deallocates the string data in x->p, and then
/       deallocates x itself.  It also marks the ischix member as
/       freed (see chxcheck).
/
/    Returns: 1
/
/    Error Conditions:  none
/  
/    Side effects:      none
/
/    Related functions: chxalloc()
/
/    Called by:  Application.
/
/    Operating system dependencies:
/       Depends on sysfree().
/
/    Known bugs:      none
/
/    Home:  chx/chxfree.c.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: JV 10/19/92 08:40 Save allocation information in file if debugging. */
/*: CT  5/28/93 16:53 Use %ld for longs. */
/*: CP  8/01/93 16:09 Use new mem_debug() for memory event recording. */
/*: OQ 02/09/98 replace chxcheck call by inline commands */
#include <stdio.h>
#include "chixdef.h"

extern int debug;

FUNCTION  chxfree (x)
   Chix   x;
{
   ENTRY ("chxfree", "");

   if (debug)  mem_debug (MEM_FREED, x->name, "chx", x->maxlen);

   /*if (NOT chxcheck (x, "chxfree"))  RETURN (1);*/
   if (x == nullchix)
      { chxerror (CXE_NULL,   "chxfree", "");        RETURN(1); }
   if (FREED(x))    
      { chxerror (CXE_FREED,  "chxfree", x->name);   RETURN(1); }
   if (NOT ISCHIX(x))
      { chxerror (CXE_NOTCHX, "chxfree", "");        RETURN(1); }

   sysfree ((char *) x->p);

   /*** Just in case some clown tries to use X... */
   x->p      = NULL;
   x->ischix = CHIX_FREED;

   sysfree ( (char *) x);

   RETURN  (1);
}
