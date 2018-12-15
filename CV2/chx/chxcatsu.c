
/*** CHXCATSUB.  Concatenate a substring of one chix to another chix.
/
/    chxcatsub (a, b, i, n);
/
/    Parameters:
/       Chix  a;    (resulting string)
/       Chix  b;    (source string)
/       int4  i;    (starting position in B of substring)
/       int4  n;    (length of desired substring)
/
/    Purpose:
/       Concatenate onto the end of A, N characters from B, starting at 
/       position I.  If type of A < type of B, A is promoted.
/
/    How it works:
/
/    Returns: 1
/
/    Error Conditions:
/       If A or B is not a chix, or A is the same as B, chxcatsub() returns
/       immediately and complains via chxerror().
/
/       Rediculous values of I or N are constrained to the beginning or end
/       of string B.
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
/    Speed: don't need size comparisons for 1 byte; may be able
/       to use memcpy instead of chxcapc.
/
/    Home:  chx/chxcatsu.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR 10/15/91 17:35 Change header comments. */
/*: JV 10/16/92 16:08 Check for a==b. */
/*: CR 12/11/92 11:05 chxcapc last arg is long. */
/*: OQ 02/09/98 replace chxcheck call by in line commands */
#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxcatsub (a, b, i, n)
   Chix   a, b;
   int4   i, n;
{
   int4   len_a, len_b, use_b;

   ENTRY ("chxcatsub", "");

   /*** Input validation. */
   /*if (NOT chxcheck (a, "chxcatsub")  ||
       NOT chxcheck (b, "chxcatsub"))     RETURN(1);*/

   if (a == nullchix || b == nullchix )
      { chxerror (CXE_NULL, "chxcat" , "");          RETURN(1); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxcat" , a->name);    RETURN(1); }
   if (FREED(b))    
      { chxerror (CXE_FREED,  "chxcat" , b->name);   RETURN(1); }
   if ( (NOT ISCHIX(a)) || (NOT ISCHIX(b)) )
      { chxerror (CXE_NOTCHX, "chxcat" , "");        RETURN(1); }
   if (a == b) {
      chxerror (CXE_SELF, "chxcatsub", a->name);
      RETURN (1);
   }
   len_a = chxlen (a);
   len_b = chxlen (b);
   if (n == ALLCHARS)  n = len_b;
   if (n <         0)  n = 0;
   if (i <         0)  i = 0;
   if (i >=    len_b)  i = len_b;
   
   /*** If type of A < type of B, or A too small to hold A + substring of B,
   /    resize A accordingly. */
   use_b = len_b - i;
   if (n < use_b)  use_b = n;
   if (a->type < b->type  ||  a->maxlen < len_a + use_b + 3)
      chxresize (a, b->type, len_a + use_b + 5);

   /*** Use chxcapc() to copy N chars from B position I to end of A. */
   chxcapc (a->p + a->type * len_a, a->type, b->p + b->type * i, b->type, n);
   a->actlen = -1L;

   RETURN  (1);
}
