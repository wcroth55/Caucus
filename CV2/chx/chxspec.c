
/*** CHXSPECIAL.   Set or fetch certain characters that have special meaning.
/
/    chxspecial (op, type, a);
/
/    Parameters:
/       int   op;      (set or get special characters)
/       int   type;    (type of special characters)
/       Chix  a;       (set or get characters to/from this chix)
/
/    Purpose:
/       Set a list of characters to have a special meaning, or get the
/       list of characters that have an assigned meaning.
/
/    How it works:
/       If 'op' is SC_SET, chxspecial() assigns property 'type' to
/       the characters in A.
/
/       If 'op' is SC_GET, it places a list of the characters that have
/       property 'type' in A.
/
/       The properties are:
/          SC_BLANK:  "blank" characters used to separate tokens in chxtoken().
/
/    Returns: 1 normally, 0 if bad 'op' or 'type'.
/
/    Error Conditions:  A not a chix (calls chxerror() to complain)
/  
/    Side effects:      none
/
/    Related functions:  chxtoken()
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  chx/chxspec.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR  7/28/91 15:48 chxalloc() first argument is long. */
/*: OQ  2/20/98 replace chxcheck by in line code */

#include <stdio.h>
#include "chixdef.h"

/*** There should be one static chix for each character 'type'. */
static Chix blanks = nullchix;

FUNCTION  chxspecial (op, type, a)
   int    op, type;
   Chix   a;
{

   ENTRY ("chxspecial", "");

   /*** Input validation. */
   if (op != SC_SET  &&  op != SC_GET)                 RETURN(0);

   /*if (NOT chxcheck (a, "chxspecial"))               RETURN(0);*/
   if (a == nullchix)
      { chxerror (CXE_NULL, "chxspecial", "");         RETURN(0); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxspecial", a->name);   RETURN(0); }
   if (NOT ISCHIX(a))
      { chxerror (CXE_NOTCHX, "chxspecial", "");       RETURN(0); }
   
   /*** Initialize static chix. */
   if (blanks == nullchix)  blanks = chxalloc (16L, THIN, "chxspecial blanks");

   if (type == SC_BLANK) {
      if (op == SC_SET)  chxcpy (blanks, a);
      else               chxcpy (a, blanks);
   }

   else  RETURN (0);

   RETURN (1);
}
