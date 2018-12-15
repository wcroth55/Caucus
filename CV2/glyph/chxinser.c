/*** CHXINSERT.   Insert a chix into another chix.
/
/    chxinsert (a, pos, b);
/
/    Parameters:
/       Chix  a;    (Into chix A...)
/       int4  pos;  (...just after this position...)
/       Chix  b;    (...insert chix B.)
/
/    Purpose:
/       Inserts chix B into chix A, at position POS.  (B gets inserted
/       immediately after the character at position POS.)
/
/    How it works:
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  application
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  glyph/chxinser.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CN  5/18/93 13:36 New function. */

#include <stdio.h>
#include "chixuse.h"

chxinsert (a, pos, b)
   Chix   a, b;
   int4   pos;
{
   int4   value;
   Chix   orig, repl;

   if (pos >= chxlen (a))  { chxcat (a, b);   return(1); }

   orig = chxalloc (L(100), THIN, "chxinser orig");
   repl = chxalloc (L(100), THIN, "chxinser repl");

   value = chxvalue (a, pos);
   chxcatval (orig, chxtype(a), value);
   chxcpy    (repl, orig);
   chxcat    (repl, b);
   chxalter  (a, pos, orig, repl);

   return (1);
}
