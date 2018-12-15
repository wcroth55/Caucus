/*** COLLAPSE_RANGE.   Collapse numeric ranges (including ALL, THIS, LAST).
/
/    ok = collapse_range (token, psuedonum, errchk, errpos);
/
/    Parameters:
/       int    ok;          (success?)
/       Token  token[];     (array of tokens, supplied by caller)
/       Chix   psuedonum;   (table of "THIS ALL LAST");
/       int    errchk;      (report errors?)
/       int   *errpos;      (put position of 1st error here)
/
/    Purpose:
/       Collapse_range() applies a filter to a set of command line item/response
/       instance tokens.  It does two things:
/
/           (a) replace all "THIS", "ALL", and "LAST" tokens with the
/               numeric codes -999, 0--1, and -1, respectively.
/
/           (b) collapses numeric ranges (such as <number><dash><number>)
/               into single tokens.
/
/    How it works:
/       If ERRCHK is true, and an error is detected, collapse_range()
/       returns 0 and sets ERRPOS to the character position of the
/       1st character in the token that caused the error.
/
/       If ERRCHK is false, collapse_range() ignores any errors, returns 1,
/       sets ERRPOS to -1, and makes its best guess at interpreting the 
/       tokens.
/
/    Returns: 1 on success
/             0 on error (if ERRCHK is true)
/
/    Error Conditions:
/       <number><dash><number><dash> is a syntax error.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  gen/collapse.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CT  5/27/93 14:11 New function. */
/*: CP  6/13/93 23:42 Mark freed chix null; use delete_token(). */
/*: CP  6/24/93 15:30 All means 0,-1 not 1,-1. */
/*: CK 10/07/93 13:46 Add ERRCHK, ERRPOS args, return code. */

#include <stdio.h>
#include "caucus.h"
#include "token.h"

FUNCTION  collapse_range (token, psuedonum, errchk, errpos)
   Token  token[];
   Chix   psuedonum;
   int    errchk;
   int   *errpos;
{
   int    this, found, which;

   ENTRY ("collapse_range", "");

   *errpos = -1;

   /*** Do the THIS/ALL/LAST substitution. */
   for (this=0;   token[this].type!=T_END;   ++this) {
      if (token[this].type != T_WORD)                              continue;
      if ( (which = tablematch (psuedonum, token[this].str)) < 0)  continue;

      switch (which) {
         case (0): token[this].num0 = -999;   token[this].num1 = -999;   break;
         case (1): token[this].num0 =    0;   token[this].num1 =   -1;   break;
         case (2): token[this].num0 =   -1;   token[this].num1 =   -1;   break;
      }
      token[this].type = T_NUM;
      chxfree (token[this].str);
      token[this].str  = nullchix;
   }

   /*** Distinguish between monadic "-" (minus sign) and dyadic "-"
   /    (range).  Look for <non-number><dash><number> and
   /    turn it into <non-number><minus number>. */
   for (this=0;   token[this].type != T_END;   ++this) {
      found = (token[this  ].type!=T_NUM  &&  token[this+1].type==T_DASH  &&
               token[this+2].type==T_NUM);
      if (NOT found)  continue;

      token[this+2].pos  = token[this+1].pos;
      token[this+2].num0 = token[this+2].num1 = - token[this+2].num0;
      delete_token (token, this+1);
   }

   /*** Collapse <number><dash><number> into <number-number>. */
   for (this=0;   token[this].type != T_END;   ++this) {
      found = (token[this  ].type==T_NUM  &&  token[this+1].type==T_DASH  &&
               token[this+2].type==T_NUM);
      if (NOT found)  continue;

      /*** Detect error case "x-y-z". */
      if (errchk  &&  token[this].num0 != token[this].num1) {
         *errpos = token[this+1].pos;
         RETURN (0);
      }

      token[this].num1 = token[this+2].num1;
      delete_token (token, this+1);
      delete_token (token, this+1);
      --this;
   }

   RETURN  (1);
}
