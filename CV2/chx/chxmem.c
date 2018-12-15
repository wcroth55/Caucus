
/*** CHXMEM.  Allocate a block of memory for chix internal functions.
/
/    p = chxmem (size, name);
/
/    Parameters:
/       char *p;
/       int4  size;        (requested size of memory block)
/       char *name;        (name associated with block by caller)
/
/    Purpose:
/       Allocate a block of SIZE bytes for use in a chix, and return a
/       pointer to that memory.  NAME is a regular C character string
/       containing the name of a chix string, used for debugging and
/       error handling.
/
/    How it works:
/       Chxmem just calls sysmem() to allocate memory off the heap.
/       It does some intelligent(?) error-handling, see below.
/
/    Returns:
/       A (char *) pointer to the allocated memory.
/
/    Error Conditions:
/       On the first call to chxmem(), it allocates (internally) a block
/       of RESERVE_SIZE bytes.  If chxmem() ever fails to allocate the
/       memory it needs, it frees the "reserve" and tries again.  If
/       the second attempt succeeds, chxmem() displays a diagnostic,
/       and tries to allocate the requested memory.
/
/       Once the reserve has been freed, if any memory allocation fails,
/       chxmem() displays an error message and exits via chxerror().
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  chxalloc(), chxresize().
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  chx/chxmem.c.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR 12/11/92 12:46 Chxmem 1st arg is long. */
/*: CR  9/18/97 13:43 Replace sysmem with calloc(). */
/*: CR  4/22/05 13:43 Replace calloc() with sysmem!. */

#include <stdio.h>
#include "chixdef.h"

#define  RESERVE_SIZE   4096L

static char  first_call = 1;   /* First call to chxmem()? */
static char *reserved;         /* Block of memory reserved for emergency. */

FUNCTION  char *chxmem (size, strname)
   int4   size;
   char  *strname;
{
   char  *p;
   char  *sysmem();

   ENTRY ("chxmem", "");

   /*** If first call to chxmem(), reserve a block of memory
   /    for memory failure emergency. */
   if (first_call) {
      first_call = 0;
      if ( (reserved = sysmem ((unsigned) RESERVE_SIZE, "emerg") == NULL) )
         chxerror (CXE_NORESV, "chxmem", "");
   }

   if ( (p = sysmem ((unsigned) size, "chxmem")) == NULL) {
      sysfree (reserved);
      reserved = NULL;
      chxerror (CXE_LOMEM, "chxmem", strname);

      if ( (p = sysmem ((unsigned) size, "chxmem1")) == NULL)  
         chxerror (CXE_NOMEM, "chxmem", "");
   }

   RETURN (p);
}
