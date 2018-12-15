
/*** INCREMENT.  Increment a sequence number.
/
/    new = increment (old, modulo);
/
/    Parameters:
/       int4   new;        (incremented sequence number)
/       int4   old;        (sequence number to increment)
/       int4   modulo;     (sequence number modulus)
/
/    Purpose:
/       Increment a sequence number, wrapping around a modulus value.
/
/       Note that even negative numbers may be incremented, although the
/       result is always in the range [0, modulo-1].  Thus, for example,
/       increment (old-2, modulo) is essentially a "decrement" of OLD.
/
/    How it works:
/
/    Returns: incremented sequence number
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
/
/    Home:  xaction/incremen.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/13/91 13:20 New function. */

#include <stdio.h>
#include "handicap.h"
#include "entryex.h"
#include "int4.h"

FUNCTION  int4 increment (old, modulo)
   int4   old, modulo;
{
   ENTRY ("increment", "");

   RETURN ( (old + modulo + 1) % modulo);
}
