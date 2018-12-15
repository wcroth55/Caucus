/*** PARSE_NEXT_NUM.   Parse the "next" number (int or double) in a string.
/
/    ok = parse_next_num (numint, numreal, is_int, precision, pos, arg);
/   
/    Parameters:
/       int     ok;        (1 if a number was found, else 0)
/       int4   *numint;    (returned integer number)
/       double *numreal;   (returned floating-point number)
/       int    *is_int;    (set to 1 if an int, else set to 0)
/       int    *precision; (number of digits to right of decimal point)
/       int4   *pos;       (look for next number starting at pos in arg)
/       Chix    arg;       (string we're looking through)
/ 
/    Purpose:
/       Parse the next number out of a string, and return its likely
/       integer and float (double) forms.  If the number is clearly a float,
/       set is_int to 0.
/
/    How it works (typical usage):
/       pos       = 0;
/       is_int    = 1;
/       precision = 0;
/       while (parse_next_num (&numint, &numreal, &is_int, &precision, 
/                              &pos, arg)) {
/          ...
/       }
/ 
/       Note that 'precision' is set to the number of digits to the
/       right of the decimal point for the most precise number parsed.
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/parsenext.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/12/01 15:50 New function. */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "chixuse.h"
#include "sweb.h"

FUNCTION  parse_next_num (
     int    *numint,
     double *numreal,
     int    *is_int,
     int    *precision,
     int4   *pos,
     Chix    arg)
{
   static Chix word = nullchix;
   int4        i;
   int         cv, state, digits, start;

   if (word == nullchix)  word = chxalloc (L(20), THIN, "parse_next_num word");

   *numint  = 0;
   *numreal = 0.;
   if (chxnextword (word, arg, pos) < 0)  return (0);

   /*** Check 1st char for +/- sign. */
   cv = chxvalue (word, L(0));
   start = (cv == '+'  ||  cv == '-') ? 1 : 0;

   /*** Scan to verify numeric syntax, decide if int/real, and count
   /    digits to right of decimal point. */
   for (digits=state=0, i=start;   (cv = chxvalue (word, i));   ++i) {
      if      (isdigit (cv)) { if (state)  ++digits; }
      else if (state == 0  &&  cv=='.')   state = 1;
      else                                return (0);
   }

   if (state == 0) {
      *numint    = atoi (ascquick(word));
      *numreal   = *numint;
   }
   else {
      *precision = max (*precision, digits);
      *is_int    = 0;
      *numreal   = atof (ascquick(word));
      *numint    = *numreal;
   }

   return (1);
}
