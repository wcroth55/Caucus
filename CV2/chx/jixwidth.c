
/*** JIXWIDTH.  Returns the width, in columns, of a "jix" character.
/
/    width = jixwidth (value);
/
/    Parameters:
/       int   width;   (returned width, either 1 or 2)
/       int4  value;   (jix character value, such as return from chxvalue())
/
/    Purpose:
/       Determine the width of a "jix" character in columns, based on
/       its value.  So far all characters are either 1 or 2 columns
/       wide.
/
/    How it works:
/       
/    Returns: width of character (1 or 2)
/
/    Error Conditions:
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  chx/jixwidth.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/14/92 14:34 New function. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  jixwidth (value)
   int4   value;
{
   int4   highbyte;

   ENTRY ("jixwidth", "");

   /*** All regular ascii, and all single-width katakana (which
   /    have high order bytes 0x8E and 0x8F) are 1 column wide.
   /    Everything else is 2 columns wide. */
   if (value < 0x80)                                RETURN (1);
   highbyte = value & 0xFF00;
   if (highbyte == 0x8E00  ||  highbyte == 0x8F00)  RETURN (1);

   RETURN (2);
}
