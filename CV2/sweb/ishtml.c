
/*** IS_HTML.   Is this an embedded HTML line?
/
/    quote = is_html (line);
/   
/    Parameters:
/       int         quote;    (position where starting quote found)
/       Chix        line;     (line to check)
/
/    Purpose:
/       CML pages may contain embedded HTML lines.  Embedded HTML
/       lines begin with some amount of whitespace, and a double
/       quote (").
/
/       Determine if this is an embedded HTML line.
/
/    How it works:
/
/    Returns: position (in LINE) of quote
/             -1 if not an HTML line
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  ishtml.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/28/95  6:59 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

FUNCTION  is_html (line)
   Chix   line;
{
   int4   pos, value;

   for (pos=0L;   (value = chxvalue (line, pos));   ++pos) {
      if (value == ' ')   continue;
      if (value == '\t')  continue;

      if (value == '"')   return ((int) pos);
      else                return (-1);
   }

   return (-1);
}
