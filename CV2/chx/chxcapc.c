
/*** CHXCAPC.  Copy (And Promote) Characters from one place to another.
/
/    chxcapc (ap, atype, bp, btype, n);
/
/    Parameters:
/       char *ap;     (pointer to destination)
/       char *bp;     (pointer to source)
/       int   atype;  (# bytes/char in destination)
/       int   btype;  (# bytes/char in source)
/       int4  n;      (# chars to copy)
/
/    Purpose:
/       Copy a string of N characters from BP to AP, promoting if
/       necessary to a larger type.
/
/    How it works:
/       BP points to a string of characters of type BTYPE, presumably part
/       of some chix.  Chxcapc() copies those characters to the memory
/       pointed to by AP.  Chxcapc() stops when it has copied N characters,
/       or when it has copied a null character (i.e., a character consisting
/       entirely of bytes of 0.)
/
/       If N < 0, Chxcapc() copies until it sees a null character.
/
/       If ATYPE > BTYPE, the characters from BP are promoted to type ATYPE
/       as they are copied.  (Characters are promoted to a larger type by 
/       adding leading zero bytes.)
/
/       The caller must guarantee that AP points to a sufficiently large
/       area of memory, that ATYPE and BTYPE are valid types, that 
/       ATYPE >= BTYPE, and that AP and BP are valid pointers.
/       
/    Returns: 1
/
/    Error Conditions:  none
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  chx...() functions only.  This is a service routine
/       for many chx functions.
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Speed: may be able to use strncpy or somesuch.
/
/    Home:  chx/chxcapc.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR 12/11/92 11:01 Make arg N long. */
/*: OQ 4/12/98 added code for single-byte character */
#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxcapc (ap, atype, bp, btype, n)
   uchar *ap, *bp;
   int    atype, btype;
   int4   n;
{
   int    deltype, i, sum;
   int4   count;

   ENTRY ("chxcapc", "");

   /*** DELTYPE is the difference in sizes between the A and B types. */

#if USE_THIN_CHIX
   /* code for one bye chixs */  
   for (count=0;   count!=n;   ++count) {
      if ( NOT (*ap++ = *bp++))  break;
   }
  *ap = '\0';

#else
   /* code for N bytes */
   deltype = atype - btype;

   /*** For each character in BP, copy it into AP, adding lead 0 bytes
   /    as needed if DELTYPE > 0.  Stop only when all bytes in a BP
   /    character are zero. */
   for (count=0;   count!=n;   ++count) {
      sum = 0;
      for (i=0;   i<deltype;   ++i)  ap[i] = '\0';
      for (i=0;   i<btype;     ++i)  {
         sum |= bp[i];
         ap [deltype + i] = bp[i];
      }

      ap += atype;
      bp += btype;
      if (sum == 0)  break;
   }

   /*** Ensure there is a null character at the end of AP. */
   for (i=0;   i<atype;   ++i)  ap[i] = '\0';
#endif
   RETURN (1);
}
