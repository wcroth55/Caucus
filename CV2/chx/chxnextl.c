
/*** CHXNEXTLINE.  Get the "next" line of a chix string.
/
/    ok = chxnextline (a, b, pos);
/
/    Parameters:
/       int   ok;     (another line found?)
/       Chix  a;      (resulting string)
/       Chix  b;      (source string)
/       int4 *pos;    (starting position in A)
/
/    Purpose:
/       Get the "next" line of a chix string B, and put it in A.
/
/    How it works:
/       Starting at position POS in B, copy all of the characters up through
/       (but not including) the next newline, into A.  Update POS to the
/       position of the first character in B after said newline.
/
/       If there are no more newlines, copy through the end of B.  Update POS
/       to the position of the 0 character at the end of B.
/
/       If the type of A is less than the type of B, A is promoted to B's
/       type.
/
/       B may be nullchix, in which case it is treated as an empty chix.
/
/    Returns: 2 if another line was found (and it ended in a newline)
/             1 if another line was found.
/             0 if POS was positioned at the end of B.
/             0 on any error.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Speed: 1-byte: pointer math instead of 'count' ?
/
/    Home:  chx/chxnextl.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/23/91 14:35 New function. */
/*: CR  6/24/91 18:29 Don't include terminating newlines. */
/*: JV  7/05/91 11:44 Fix docs. */
/*: CR  7/23/91 20:04 Allow B to be nullchix.  Set actlen. */
/*: CR 10/15/91 18:44 Return 2 if line ended in newline. */
/*: CR 12/11/92 11:05 chxcapc last arg is long.  POS arg is long. */
/*: OQ  2/15/98 replace chxcheck by in line code */
/*: OQ  3/01/98 added code for single byte charater chix type */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxnextline (a, b, pos)
   Chix   a, b;
   int4  *pos;
{
   uchar *bp;
   int    sum, i, newline;
   int4   count;

   ENTRY ("chxnextline", "");

   /*** Input validation. */
   /*if (NOT chxcheck (a, "chxnextline"))               RETURN (0);*/
   if (a == nullchix)
      { chxerror (CXE_NULL, "chxnextline", "");         RETURN(0); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxnextline", a->name);   RETURN(0); }
   if (NOT ISCHIX(a))
      { chxerror (CXE_NOTCHX, "chxnextline", "");       RETURN(0); }
   if (b == nullchix) 
      { chxclear (a);                                   RETURN (0); }

   /*if (NOT chxcheck (b, "chxnextline"))   RETURN (0);*/
   if (FREED(b))    
      { chxerror (CXE_FREED, "chxnextline", b->name);   RETURN(0); }
   if (NOT ISCHIX(b))
      { chxerror (CXE_NOTCHX, "chxnextline", "");       RETURN(0); }

   /*** Starting from POS, find number of characters COUNT left in B
   /    until the next newline (or end of string). */
#if USE_THIN_CHIX
   bp = b->p + (*pos);
   for (newline=count=0;   (1);   bp++, ++count) {

/*     sum = 0;
      sum = *bp & 0xFF;
      if  (sum == '\n'  &&  *bp == '\n')  { newline=1;   break; }
      if  (sum ==  0)                                        break;
*/
      if  (*bp == '\n')  { newline=1;   break; }
      if  (NOT *bp    )                 break;

   }
   if (count + newline == 0)  RETURN (0);
      
   /*** Ensure A is proper size and type. */
   chxclear (a);
   if (/*a->type < b->type  || */  a->maxlen < count+5) 
      chxresize (a, b->type, count+10);

   /*** Copy COUNT characters from B:POS to A.  Update POS. */
   chxcapc (a->p, a->type, b->p + *pos , b->type, count);

#else   
   bp = b->p + b->type * (*pos);
   for (newline=count=0;   (1);   bp += b->type, ++count) {
      sum = 0;
      for (i=0;   i<b->type;   ++i)  sum += (bp[i] & 0xFF);
      if  (sum == '\n'  &&  bp[i-1] == '\n')  { newline=1;   break; }
      if  (sum ==  0)                                        break;
   }
   if (count + newline == 0)  RETURN (0);
      
   /*** Ensure A is proper size and type. */
   chxclear (a);
   if (a->type < b->type  ||  a->maxlen < count+5) 
      chxresize (a, b->type, count+10);
   /*** Copy COUNT characters from B:POS to A.  Update POS. */
   chxcapc (a->p, a->type, b->p + b->type * (*pos), b->type, count);
#endif

   *pos += count + newline;
   a->actlen = -1;

   RETURN (1 + newline);
}
