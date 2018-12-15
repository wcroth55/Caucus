
/*** CHXINT4.  Scan a chix for a int4 integer.
/
/    v = chxint4 (a, i);
/
/    Parameters:
/       int4  v;
/       Chix  a;
/       int4 *i;    (scan from this character position in A.)
/
/    Purpose:
/       Scan a chix for an integer.
/
/    How it works:
/       Scan chix A, starting at position I, for an integer.  The integer
/       may consist of leading blanks and a leading plus or minus sign, 
/       followed by the digits 0 through 9.  I is set to the position of 
/       the first character in A after the integer.
/
/       If an error occurs, or there is no integer at position I, I is
/       unchanged.
/
/    Returns: value of integer
/             (undefined on error)
/
/    Error Conditions:
/       A not a chix
/       I is NULL
/      *I is out of range
/  
/    Side effects:
/
/    Related functions: chxnum().
/
/    Called by:  Application, chxnum().
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Speed: 1-byte chxvalues could be replaced with array reference.
/
/    Home:  chx/chxint4.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR  7/23/91 23:19 Allow leading '+' sign. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: OQ 02/10/98 replace chxcheck with in line codea */

#include <stdio.h>
#include "chixdef.h"

#define  DIGIT(x)   ('0'<=x  &&  x<='9')

FUNCTION  int4 chxint4 (a, i)
   Chix   a;
   int4  *i;
{
   static char  blanktab[256];
   static int4 *blanklist = NULL;
   Chix         blanks;
   int4         j, j0, sp, cval;
   int          space, minus = 0;
   int4         result, c, sign;
   char        *sysmem();

   ENTRY ("chxint4", "");

   /*** Initialization: Get the list of "blank" characters.  If it's 
   /    empty, use the standard ascii blank, ' '. */
   if (blanklist == NULL) {
      blanks = chxalloc (16L, THIN, "chxtoken blanks");
      chxspecial (SC_GET, SC_BLANK, blanks);
      if (chxvalue (blanks, 0L) == 0L)  chxcatval (blanks, THIN, (int4) ' ');
      blanklist = (int4 *) sysmem (L(sizeof(int4) * (chxlen (blanks) + 2)), "int4");
      for (j=0;   (blanklist[j] = chxvalue (blanks, L(j)));   ++j)  ;
      for (j=0;   j < 256;        ++j)  blanktab[j] = 0;
      for (j=0;   blanklist[j];   ++j)  {
         if (blanklist[j] < 256)  blanktab[blanklist[j]] = 1;
      }
      chxfree (blanks);
   }

   /*** Argument checking.  */
   /*if (NOT chxcheck (a, "chxint4")  ||
         i == (int4 *) NULL           ||
        *i <  0                       ||
        *i >= a->maxlen)                          RETURN (0L);*/

   if (a == nullchix)
      { chxerror (CXE_NULL, "chxint4", "");       RETURN(0L); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxint4", a->name); RETURN(0L); }
   if (NOT ISCHIX(a))
      { chxerror (CXE_NOTCHX, "chxint4", "");     RETURN(0L); }
   if( i == (int4 *) NULL           ||
      *i <  0                       ||
      *i >= a->maxlen)                            RETURN (0L);
   


   /*** Skip over any leading blank characters (of any sort). */
   j = *i;
/* while (chxvalue (a, j) == ' ')   ++j; */

#if USE_THIN_CHIX
   while (blanktab [chxvalue(a, j)])   ++j;
#else
   while (1) {
      cval = chxvalue (a, j);
      if (cval < 256  &&  blanktab[cval])  { ++j;       continue; }
      for (space=sp=0;   blanklist[sp];   ++sp)
         if (cval == blanklist[sp])        { space = 1;    break; }
      if (NOT space)  break;
      ++j;
   }
#endif

   /*** Now pick up any sign characters. */
   sign = chxvalue (a, j);
   if (sign == '-')               { ++j;   minus = 1; }
   if (sign == '+')                 ++j;

   for (j0=j, result=L(0);   c = chxvalue (a, j), DIGIT(c);   ++j) 
      result = L(10) * result + (c - '0');
   
   if (j0 == j)                          RETURN (L(0));
   *i = j;
   RETURN (minus ? -result : result);
}
