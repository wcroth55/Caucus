
/*** CHXOFASCII.  Convert ascii string to chix.
/
/    chxofascii (a, str);
/
/    Parameters:
/       Chix  a;
/       char *str;     (ascii string to be converted)
/
/    Purpose:
/       Convert a regular "C" string of ASCII characters (an array of
/       bytes, terminated by a 0 byte) in STR, to a chix A.
/   
/    How it works:
/
/    Returns: 1 if successful, 0 on error.
/
/    Error Conditions:
/       A not chix.
/       STR is NULL.
/       A cannot be expanded.
/  
/    Side effects:
/
/    Related functions: chxquick().
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  chx/chxofasc.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/24/91 14:45 New function. */
/*: CR  1/03/92 13:34 Fail if A cannot be expanded. */
/*: CR 12/11/92 11:05 chxcapc last arg is long. */
/*: OQ  2/13/98 replace chxcheck call by in line codes */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxofascii (a, str)
   Chix   a;
   char  *str;
{
   int4   len;

   ENTRY ("chxofascii", "");

   /*** Input validation. */
   /*if (NOT chxcheck (a, "chxofascii"))          RETURN (0);*/
   if (a == nullchix)
      { chxerror (CXE_NULL, "chxlofascii", "");        RETURN(0); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxlofascii", a->name);  RETURN(0); }
   if (NOT ISCHIX(a))
      { chxerror (CXE_NOTCHX, "chxlofascii", "");      RETURN(0); }
   if (str == NULL)                             RETURN (0);
   
   len = strlen (str);
   if (a->maxlen < len + 5)
      if (NOT chxresize (a, a->type, len + 5))  RETURN (0);

   chxclear (a);
   chxcapc  (a->p, a->type, (uchar *) str, THIN, -1L);
   a->actlen = len;

   RETURN (1);
}
