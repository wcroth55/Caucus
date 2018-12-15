
/*** CHXSCAN.  Scan across parts of two chix strings until they are not equal.
/
/    n = chxscan (ap, atype, bp, btype, enda, endb);
/
/    Parameters:
/       int4  n;       (number of characters scanned)
/       char *ap;      (pointer to first character in A chix)
/       int   atype;   (type of A chix)
/       char *bp;      (pointer to first character in B chix)
/       int   btype;   (type of B chix)
/       int4 *enda;    (long value of not-equal character in A)
/       int4 *endb;    (long value of not-equal character in B)
/
/    Purpose:
/       Chxscan() is a chix service function, used by higher-level
/       chix functions such as chxeq().  It compares two chix strings,
/       finds where they differ, and returns information about where
/       and how they differ.
/
/    How it works:
/       Chxscan() scans and compares the text of two chix, pointed to by AP 
/       and BP.  (It uses the types ATYPE and BTYPE to automatically promote
/       individual characters as needed so that they can be compared.)
/       It stops when it finds a difference (or at the end of the chix if
/       they are identical).
/
/       Because chxscan() is an internal chix function, it does not do
/       any validation of the input arguments.
/
/    Returns: N, the index of the differing character.  (i.e., if the 3rd
/       characters differ, chxscan() returns 2.)
/
/       Also puts the value of the N'th A character in ENDA as a long,
/       and  puts the value of the N'th B character in ENDB.
/
/    Error Conditions:  none
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  chx...() functions only, never by application.
/
/    Operating system dependencies:
/
/    Known bugs:
/       Limitation: requires that a character in a chix be no more than 
/       sizeof(int4) bytes, typically 4.
/
/    Speed: 1-byte case, pointer math?
/
/    Home:  chx/chxscan.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR 12/11/92 13:04 Returns long. */
/*: OQ 03/04/98 added new code for single byte character chix type */
/*: CR 06/23/98 speed: register variables, simplify if (ZERO) case. */

#include <stdio.h>
#include "chixdef.h"

#define  ZERO(x)  (! (x) )

FUNCTION  int4 chxscan (ap, atype, bp, btype, enda, endb)
   register uchar *ap, *bp;
   int             atype, btype;
   int4           *enda, *endb;
{
   register int4 n;
   int4          a, b;
   int           i;

   ENTRY ("chxscan", "");

#if USE_THIN_CHIX
   for (n=0;   (1);   ++n) {
/*    if (*ap != *bp  ||  (ZERO(*ap)  &&  ZERO(*bp)) )  break; */
      if (*ap != *bp  ||   ZERO(*ap)                 )  break;
      ++ap;
      ++bp;
   }
   *enda = *ap;
   *endb = *bp;

#else
   for (n=0;   (1);   ++n) {
      a = *ap++;
      for (i=1;   i<atype;   ++i)  { a = a << 8;   a += *ap++; }
      b = *bp++;
      for (i=1;   i<btype;   ++i)  { b = b << 8;   b += *bp++; }

      if (a != b  ||  (a==0L  &&  b==0L))  break;
   }
   *enda = a;
   *endb = b;
#endif

   RETURN (n);
}
