/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSMEM.  Allocate a block of B bytes of memory.
/
/    SYSMEM allocates the requested block, aligned for any data type,
/    and RETURNs a pointer to the block.  SYSMEM will almost certainly
/    always be just a call to MALLOC, but just in case... */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR  4/11/07 Reverse order of calloc args! */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#define  DEBUG 0

long memused = 0;

FUNCTION  char *sysmem (int4 b, char *a)
{
#if UNIX | NUT40 | WNT40
   char   *calloc();
   char   *p;
   union {unsigned char c[4];   int4 l; }  u;

#if DEBUG
   p = calloc ((unsigned) (b + 6), 1);
   memused += b;
   u.l  = b;
   p[0] = u.c[0];
   p[1] = u.c[1];
   p[2] = u.c[2];
   p[3] = u.c[3];
   return (p+4);
#else
   return calloc ((unsigned) (b + 2), 1);
#endif

#endif
}
