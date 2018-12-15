
/*** JIXSINGLE.  Convert a double-width jix char to its single-width form.
/
/    ok = jixsingle (hi, lo);
/
/    Parameters:
/       int   ok;      (was this character converted?)
/       int  *hi;      (high byte of character)
/       int  *lo;      (low  byte of character)
/
/    Purpose:
/       The "jix" character set contains many characters which are
/       double-width equivalents of standard single-width ASCII characters.
/       Jixsingle() determines if this is one such character, and if
/       so, converts it to the single-width form.
/
/    How it works:
/       HI is the high order byte of the character; LO the low-order byte.
/       If the character can be converted, HI is set to 0, and LO is 
/       set to the ascii equivalent.
/
/    Returns: 1 if the character was converted, and 0 otherwise. 
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  ascofchx(), jixreduce()
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  chx/jixsingl.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/15/92 14:41 New function. */

#include <stdio.h>
#include "chixdef.h"

#define  NOEQ  0    /* Translation map has no equivalent. */

/*** Map of JIS segment 1 to ascii punctuation chars. */
static char puncmap[] = {
  0x20, 0x2C, 0x2E, 0x2C, 0x2E, NOEQ, 0x3A, 0x3B,
  0x3F, 0x21, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, 0x5E,
  0x7E, 0x5F, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, 0x2D, 0x2D, 0x2F, 0x5C,
  0x7E, 0x7C, 0x7C, NOEQ, NOEQ, 0x60, 0x27, 0x22,
  0x22, 0x28, 0x29, 0x5B, 0x5D, 0x5B, 0x5D, 0x7B,
  0x7D, 0x3C, 0x3E, 0x3C, 0x3D, NOEQ, NOEQ, NOEQ,
  NOEQ, 0x28, 0x29, 0x2B, 0x2D, NOEQ, NOEQ, NOEQ,
  0x3D, 0x3D, 0x3C, 0x3E, 0x3C, 0x3E, NOEQ, NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, 0x5C, 0x24,
  NOEQ, NOEQ, 0x25, 0x23, 0x26, 0x2A, 0x40, NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ
};

FUNCTION  jixsingle (hi, lo)
   int   *hi, *lo;
{
   int    ascii;

   ENTRY ("jixsingle", "");

   /*** Double-width punctuation to single-width ascii. */
   if (*hi == 0xA1) {
      ascii = puncmap [*lo - 0xA1];
      if (ascii > 0)  {  *hi = 0;   *lo = ascii;                RETURN (1); }
   }

   /*** Double-width alphanumerics to single-width ascii. */
   if (*hi == 0xA3)   {  *hi = 0;   *lo = *lo - 0xA0 + 0x20;    RETURN (1); }

   RETURN (0);
}
