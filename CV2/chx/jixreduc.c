
/*** JIXREDUCE.  Reduce a jix string to its "lower case" form.
/
/    ok = jixreduce (a);
/
/    Parameters:
/       int  ok;       (1 on success, 0 on failure)
/       Chix  a;       (jix string to be reduced)
/
/    Purpose:
/       Jixreduce() is the jix equivalent of strlower().  It maps certain
/       jix characters into other characters.  This reduction makes life
/       easier for other functions that try to compare jix from different
/       sources.
/
/    How it works:
/       Specifically, jixreduce() maps:
/
/       (a) double-width punctuation to single-width ascii punctuation.
/       (b) double-width alphanumerics to single-width ascii alphanumerics.
/       (c) upper-case ascii letters to lower-case ascii letters.
/       (d) single-width katakana to double-width katakana.
/       (e) small size dw-katakana to large size dw-katakana.
/       (f) THIN upper-case ascii letters to THIN lower-case ascii letters.
/
/    Returns: 1 on success, 0 on error.
/
/    Error Conditions: A is not a chix.
/  
/    Side effects:
/
/    Related functions: jixsingle()
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:
/       Currently jixreduce() only reduces upper-case ASCII.
/       It should be changed to know about other character sets
/       that support two cases for each character.
/       It might be sufficient to support Latin-1.
/
/    Home:  chx/jixreduc.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/24/91 14:45 New function. */
/*: CR  6/18/91 17:44 Fix error: use 0xA0 for base, not 0x80. */
/*: CR  1/03/92 13:42 Return 0 on error. */
/*: CX  5/15/92 14:48 Strip out jixsingle() as separate function. */
/*: JX  5/29/92 13:24 Remove variable ASCII. */
/*: JX  6/10/92 20:36 Begin THIN jixreduce(). */

#include <stdio.h>
#include "chixdef.h"

#define  NOEQ  0    /* Translation map has no equivalent. */

/*** Map swkana followed by " into JIS segment 5 offset. */
static char swkana63map[] = {
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ,
  NOEQ, NOEQ, NOEQ,  84,  NOEQ, NOEQ,  12,   14,
   16,   18,   20,   22,   24,   26,   28,   30,
   32,   34,   36,   38,   40,  NOEQ, NOEQ, NOEQ,
  NOEQ, NOEQ,  48,   51,   54,   57,   60,  NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ
};

/*** Map swkana followed by circle into JIS segment 5 offset. */
static char swkana64map[] = {
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ,
  NOEQ, NOEQ,  49,   52,   55,   58,   61,  NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ,
  NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ, NOEQ
};

/*** Map remaining sw-kana into JIS segment,offset number. */
static char swkana[][2] = {
  1,1,  1,3,  1,54, 1,55, 1,2,  1,6,  5,82, 5,1,
  5,3,  5,5,  5,7,  5,9,  5,67, 5,69, 5,71, 5,35,
  1,28, 5,2,  5,4,  5,6,  5,8,  5,10, 5,11, 5,13,
  5,15, 5,17, 5,19, 5,21, 5,23, 5,25, 5,27, 5,29,
  5,31, 5,33, 5,36, 5,38, 5,40, 5,42, 5,43, 5,44,
  5,45, 5,46, 5,47, 5,50, 5,53, 5,56, 5,59, 5,62,
  5,63, 5,64, 5,65, 5,66, 5,68, 5,70, 5,72, 5,73,
  5,74, 5,75, 5,76, 5,77, 5,79, 5,83, 1,11, 1,12
};

/*** Map small size dw-katakana to large size dw-katakana. */
static char dw2dw[][2] = {
  1,2,   3,4,   5,6,   7,8,  9,10, 35,36, 67,68, 69,70, 71,72, 
  78,79, 85,11, 86,17, 0,0
};

FUNCTION  jixreduce (a)
   Chix   a;
{
   int    i, j, hi, lo, offset, skip, nexthi, nextlo;

   ENTRY ("jixreduce", "");

   if (NOT chxcheck (a, "jixreduce"))                    RETURN (0);

   if (a->type == THIN)  {
      /*** Upper case letters to lower case letters. */
      for (i=0;   a->p[i];   i++) 
         if (a->p[i] >='A' &&  a->p[i] <='Z')
            a->p[i] = a->p[i] - 'A' + 'a';

   } else
   /*** For each character in A, pick up the HI and LO bytes. */
   for (i=0;   (hi = a->p[i] & 0xFF) | (lo = a->p[i+1] & 0xFF);   i+=2) {
      skip = 0;

      /*** Convert double-width equivalents of ascii chars into their
      /    single-width form.  (Punctuation, alphanumerics.)  */
      jixsingle (&hi, &lo);

      /*** Upper case letters to lower case letters. */
      if (hi == 0  &&  lo>='A' &&  lo<='Z')  lo = lo - 'A' + 'a';

      /*** Single-width katakana to double-width katakana. */
      if (hi == 0x8E) {

         /*** See if the next character is swkana " or circle. */
         nexthi = a->p[i+2] & 0xFF;
         nextlo = a->p[i+3] & 0xFF;

         if (nexthi == 0x8E  &&  nextlo == 0xDE) {
            offset = swkana63map [lo - 0xA0];
            if (offset != NOEQ) { hi = 0xA5;   lo = offset + 0xA0;   skip=1; }
         }

         if (nexthi == 0x8E  &&  nextlo == 0xDF) {
            offset = swkana64map [lo - 0xA0];
            if (offset != NOEQ) { hi = 0xA5;   lo = offset + 0xA0;   skip=1; }
         }

         /*** Now map any swkana not already mapped by having a following 
         /    " or circle. */
         if (NOT skip) {
            hi = swkana [lo - 0xA0][0] + 0xA0;
            lo = swkana [lo - 0xA0][1] + 0xA0;
         }
      }

      /*** Small size dw-katakana to large size dw-katakana. */
      if (hi == 0xA5)   for (j=0;   dw2dw[j][0];   ++j) {
         if (lo - 0xA0 == dw2dw[j][0])  { lo = dw2dw[j][1] + 0xA0;  break; }
      }

      /*** If SKIP is true, delete this character ("skipping" it) and
      /    move the rest of the chix left one character position. */
      if (skip)   for (j=i;   a->p[j] | a->p[j+1];   j+=2) {
         a->p[j  ] = a->p[j+2];
         a->p[j+1] = a->p[j+3];
      }
         
      /*** Put the changes back into the chix. */
      a->p[i  ] = hi;
      a->p[i+1] = lo;
   }

   RETURN (1);
}
