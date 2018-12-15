/*** REPORT_PARSE_ERR.   Tell user about error from parsing command line.
/
/    report_parse_err (range, errpos);
/
/    Parameters:
/       Chix   range;       (range supplied by user, containing error)
/       int    errpos;      (character number of start of error in RANGE)
/
/    Purpose:
/       Report an error parsing a command line.
/
/    How it works:
/       RANGE is an item/response range, entered by the user, that
/       contained some errors (as reported by new_parse()).  Tell the
/       user about the error, and point at the character that started
/       the offending token.
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  parse_or_ask(), other Caucus CLI functions.
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  gen/reportpa.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CK 10/07/93 14:50 New function. */

#include <stdio.h>
#include "caucus.h"

extern union null_t           null;

FUNCTION  report_parse_err (range, errpos)
   Chix   range;
   int    errpos;
{
   int    columns;
   int4   pos;
   Chix   format, errstr, pointer;
   char   ftemp[20], ptemp[200];

   ENTRY ("report_parse_err", "");

   format  = chxalloc (L(200), THIN, "report format");
   errstr  = chxalloc (L(200), THIN, "report errstr");
   pointer = chxalloc (L(200), THIN, "report pointer");

   /*** Figure out how many single-width columns it takes to go out
   /    to character position ERRPOS. */
   for (columns=0, pos=0;   pos < errpos;   ++pos)
      columns += jixwidth (chxvalue (range, pos));

   /*** Create a string that has that many columns and a '^' pointer. */
   sprintf (ftemp, "%%%ds", columns+1);
   sprintf (ptemp, ftemp, "^");

   /*** Write out the RANGE and the error pointer. */
   mdstr (format, "res_Ferrparse", null.md);
   chxformat  (errstr, format, L(0), L(0), range, CQ(ptemp));
   unit_write (XWER, errstr);

   chxfree (format);
   chxfree (errstr);
   chxfree (pointer);

   RETURN  (1);
}
