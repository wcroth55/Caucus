
/*** GREATER_THAN.  Is one sequence number greater (higher) than another?
/
/    yes = greater_than (a, b, modulo);
/
/    Parameters:
/       int    yes;        (is a greater than b?)
/       int4   a;          (first sequence number)
/       int4   b;          (second sequence number)
/       int4   modulo;     (sequence number modulus)
/
/    Purpose:
/       Compare two cyclic sequence numbers, and determine if A is
/       "greater than" B.
/
/    How it works:
/       Greater_than() returns true if A > B (allowing for wrap around
/       the end of modulus), and is within modulo/2 of B.
/
/    Returns: 1 if true, else 0.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Called by:
/
/    Related functions:
/
/    Operating system dependencies:
/
/    Known bugs:
/       This algorithm effectively only lets us use half of the "modulo"
/       space.  Better algorithms exist that require more information or
/       history.
/
/    Home:  xaction/greater.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/13/91 13:20 New function. */

#include <stdio.h>
#include "handicap.h"
#include "entryex.h"
#include "int4.h"

FUNCTION  greater_than (a, b, modulo)
   int4   a, b, modulo;
{
   ENTRY ("greater_than", "");

   if (b < a  &&  a < b + modulo/2)  RETURN (1);
   a += modulo;
   if (b < a  &&  a < b + modulo/2)  RETURN (1);
   RETURN (0);
}
