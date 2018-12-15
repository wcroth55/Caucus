
/*** PRINT_RLIST.   Display the contents of an Rlist.
/
/    print_rlist (text, rla);
/
/    Parameters:
/       char     *text;     (text to be displayed)
/       Rlist     rla;      (Rlist to be printed)
/
/    Purpose:
/       Debugging tool for displaying an Rlist.
/
/    How it works:
/
/    Note:
/
/    Returns: 1
/
/    Error Conditions:
/
/    Side effects: 
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/printrl.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  5/17/93 21:31 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

#define  RLNULL     ((Rlist) NULL)

FUNCTION  print_rlist (char *text, Rlist rla)
{
   Rlist  pa;

   if (rla==RLNULL)  return (1);

   fprintf (stderr, "%s\n----------------------\n", text);
   for (pa=rla->next;   pa!=RLNULL;   pa=pa->next) {
      fprintf (stderr, " %4d:%4d  ->  %4d:%4d\n", pa->i0, pa->r0, 
                                                  pa->i1, pa->r1);
   }

   return (1);
}
