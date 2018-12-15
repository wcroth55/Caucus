/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSFREE.  Deallocate a block of memory allocated by SYSMEM.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#define  DEBUG 0

extern long memused;

FUNCTION  sysfree (unsigned char *p)
{
   union {unsigned char c[4];   int4 l; }  u;

   if (p != NULL)  {

#if DEBUG
      p = p - 4;
      u.c[0] = p[0];
      u.c[1] = p[1];
      u.c[2] = p[2];
      u.c[3] = p[3];
      memused -= u.l;
#endif

      free (p); 
   }
}
