
/*** ASCOFCHX.   Convert a chix to an ASCII string.
/
/    conv = ascofchx (s, a, i, n);
/
/    Parameters:
/       int4  conv;  (number of A's characters converted)
/       char *s;     (Where to place the ascii string.)
/       Chix  a;     (Chix to be converted.)
/       int4  i;     (Start at character position I in A.)
/       int4  n;     (Convert as much as will fit in N bytes.)
/
/    Purpose:
/       Convert (a substring of) a chix string A into a stream of ASCII
/       bytes, in S.
/
/    How it works:
/       Start at character position I in A.  Convert as much of A as will
/       fit into N bytes of S (including placing a terminating null byte
/       into S.)   If I < 0, uses I=0.
/
/       If a character is outside regular (0-255) ASCII, convert as
/       appropriate to ASCII.  (Double-width equivalents of ascii go
/       to single-width ascii plus padding.  Single-width non-ascii
/       go to "*".  Double-width non-ascii go to "[]".)
/       
/    Returns:  
/       Number of characters from A that were converted.
/       0 if *all* of A (from position I on) was converted.
/      -1 on error.
/
/    Error Conditions:
/       S is NULL.
/       A is not chix (sets S to the empty string).
/       Type of A is not 1 or 2.
/       N <= 0                    (ditto)
/       I too large.              (ditto)
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:
/       2-byte chix are assumed to be "jix" format.  This is
/       iconoclastic, at the least.
/
/       The conversion of non-ascii characters to "*" and "[]" is
/       rather arbitrary, and should someday be made more flexible.
/
/    Speed: use memcpy for 1-byte for loop.
/
/    Home:  chx/ascofchx.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/24/91 14:45 New function. */
/*: CR  6/19/91 14:16 Fix "i+j" bug. */
/*: CX  5/15/92 15:23 Handle 2-byte chix conversions. */
/*: JX  5/29/92 14:21 Cast arg to jixwidth, add handicap.h. */
/*: CX  6/05/92 17:21 Remove handicap.h. */
/*: CX  6/26/92 15:45 Fix logic error in mapping kanji -> ascii. */
/*: CR 12/10/92 15:24 Change to use & return long. */
/*: OQ 02/15/98 Replace chxcheck call by inline commands */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  int4 ascofchx (s, a, i, n)
   char  *s;
   Chix   a;
   int4   i, n;
{
   int    hi, lo;
   int4   j, conv;
   uchar *ptr;

   ENTRY ("ascofchx", "");

   /*** Input validation. */
   if (s == NULL)                     RETURN (-1);
   *s = '\0';
   /*if (NOT chxcheck (a, "ascofchx"))  RETURN (-1);*/
   if (a == nullchix)
                     { chxerror (CXE_NULL,   "ascofchx", "");        RETURN(-1); }
   if (FREED(a))    
                     { chxerror (CXE_FREED,  "ascofchx", a->name);   RETURN(-1); }
   if (NOT ISCHIX(a))
                     { chxerror (CXE_NOTCHX, "ascofchx", "");        RETURN(-1); }
   if (a->type > 2)                   RETURN (-1);
   if (i >= a->maxlen)                RETURN (-1);
   if (i < 0)  i = 0;
   if (n == ALLCHARS)  n = a->maxlen;
   
   /*** Do single-byte ascii chix case; it's easy. */
   /*if (a->type == 1) {
      for (j=0;    a->p[i+j]  &&  j<n-1;   ++j)  s[j] = a->p[i+j];
      s[j] = '\0';
      RETURN (a->p[i+j] ? j : 0);
   }*/
#if USE_THIN_CHIX
      for (j=0;    a->p[i+j]  &&  j<n-1;   ++j)  s[j] = a->p[i+j];
      s[j] = '\0';
      RETURN (a->p[i+j] ? j : 0);

#else
   /*** The rest of the code assumes 2-byte "jix" characters.
   /    Someday we may need to expand beyond this assumption. */

   /*** PTR points to start of I'th character.  Just increment this as
   /    we scan through the chix. */
   ptr = a->type * i + a->p;

   /*** Keep converting until we run out of room in the ascii buffer.
   /    CONV is count of characters converted. */
   for (conv=j=0;   j<n-1;   ) {
      
      /*** Get the high & low bytes; all 0 is end of chix string. */
      hi = *ptr++;
      lo = *ptr++;
      if (hi + lo == 0)  { s[j] = '\0';   RETURN (0); }
      ++conv;

      /*** One-byte char stored in 2-byte chix; easy. */
      if (hi==0) { s[j++] = lo;   continue; }

      /*** There are three possible cases for 2-byte data:
      /    (a) width-2 equivalent of ascii character => ascii + ' '/'_'
      /    (b) single-width katakana => '*'
      /    (c) double-width katakana or kanji => '[]'. */
      if (jixsingle (&hi, &lo)) {
         s[j++] =  lo;
/*       s[j++] = (lo==' ' ? ' ' : '_'); */
         s[j++] = ' ';
      }
      else if (jixwidth (L(hi * 256 + lo)) == 1)   s[j++] = '*';
      else                       { s[j++] = '[';   s[j++] = ']'; }
   }
   s[j] = '\0';

   RETURN (conv);
#endif
}
