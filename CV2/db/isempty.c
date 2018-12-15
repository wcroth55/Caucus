/*** IS_EMPTY.   Is a Textbuf empty (of responses or messages) in a range?
/
/    empty = is_empty (tbuf, r0, r1);
/
/    Parameters:
/       int    empty;       (1 if empty, 0 else)
/
/    Purpose:
/       Determine if part of a textbuf is empty (has only deleted
/       responses or messages in it).
/
/    How it works:
/
/    Returns: 1 if the selected responses are deleted
/             0 otherwise.
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/isempty.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/14/95 18:12 New function. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  is_empty (tbuf, r0, r1)
   Textbuf  tbuf;
   int      r0, r1;
{
   int      b;

   ENTRY ("is_empty", "");

   /*** Is the selected range of the tbuf (partfile) empty? */
   for (b=r0;   b<=r1;   ++b) {
      if (tbuf->r_stop[b] > tbuf->r_start[b])  RETURN (0);
   }

   RETURN (1);
}
