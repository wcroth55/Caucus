
/*** MEM_DEBUG.   Record memory events for debugging purposes. 
/
/    mem_debug (event, name, kind, size);
/
/    Parameters:
/       int   event;      (event type: MEM_ALLOC, MEM_FREED...)
/       char *name;       (name of memory object)
/       char *kind;       (kind of memory object)
/       int4  size;       (size of memory object, if relevant)
/
/    Purpose:
/       Record memory events (in a file) for debugging purposes.
/
/    How it works:
/       Events of type MEM_ALLOC are recorded in the file memalloc.out
/       in the user's current directory.
/
/       Events of type MEM_FREED are recorded in the file memfree.out
/       in the user's current directory.
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions: 
/
/    Called by:  chxalloc(), chxfree(), make_text()...
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  chx/memdebug.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  8/01/93 16:05 New function. */
/*: CP  9/02/93 17:09 Use sysprv() around fopen() calls. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  mem_debug (event, name, kind, size)
   int    event;
   char  *name, *kind;
   int4   size;
{
   static FILE *f_aloc = NULL;
   static FILE *f_free = NULL;

   if (event == MEM_ALLOC) {
      if (f_aloc == NULL) {
         sysprv(0);
         f_aloc = fopen ("memalloc.out", "a");
         sysprv(1);
      }
      if (f_aloc == NULL)  return (0);
      fprintf (f_aloc, "%s %s:%ld\n", name, kind, (long) size);
      fflush  (f_aloc); 
   }

   if (event == MEM_FREED) {
      if (f_free == NULL) {
         sysprv(0);
         f_free = fopen ("memfree.out", "a");
         sysprv(1);
      }
      if (f_free == NULL)  return (0);
      fprintf (f_free, "%s %s:%ld\n", name, kind, (long) size);
      fflush  (f_free); 
   }

   return (1);
}
