
/*** MERGE_TOKENS.   Merge simple tokens into full item:resp specs.
/
/    ok = merge_tokens (token, errchk, errpos);
/
/    Parameters:
/       Token  token[];     (array of tokens, supplied by caller)
/       int    errchk;      (report errors?)
/       int   *errpos;      (put position of 1st error here)
/
/    Purpose:
/       Merge_tokens() is handed an array of "simple" tokens, that
/       include:
/          numeric range
/          keyword (e.g. "NEW" or "AUTHOR")
/          data string (for keywords like "AUTHOR")
/          a colon
/
/       and merges these tokens into a more complex set that contain:
/          item specification
/          item specification : range specification
/          keyword
/          
/    How it works:
/       If ERRCHK is true, and an error is detected, merge_tokens()
/       returns 0 and sets ERRPOS to the character position of the
/       1st character in the token that caused the error.
/
/       If ERRCHK is false, merge_tokens() ignores any errors, returns 1,
/       sets ERRPOS to -1, and makes its best guess at interpreting the 
/       tokens.
/
/    Returns: 1 on success
/             0 on error (if ERRCHK is true)
/
/    Error Conditions:
/       monadic ':'
/       ' a:: '
/       ' bad_keyword "data" '
/       
/    Side effects:
/
/    Related functions:
/
/    Called by:  new_parse()
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  gen/mergetok.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  6/13/93 22:41 New function. */
/*: CP  8/05/93 14:49 Handle monadic ":". */
/*: CK 10/07/93 13:56 Add ERRCHK, ERRPOS args, return code. */

#include <stdio.h>
#include "caucus.h"
#include "token.h"

#define  ERROR(x)  { *errpos = x;   RETURN(0); }

FUNCTION  merge_tokens (token, errchk, errpos)
   Token  token[];
   int    errchk;
   int   *errpos;
{
   int    this;

   ENTRY ("merge_tokens", "");

   *errpos = -1;
   if (token[0].type == T_END)  RETURN (1);

   /*** Weird case of monadic colon, " : n" becomes error, or else "all:n". */
   if (token[0].type == T_COLON) {
      if (errchk)          ERROR (token[0].pos);
      token[0].type     =  T_NUM;
      token[0].num0     =  1;
      token[0].num1     = -1;
      token[0].hascolon =  1;
   }

   for (this=1;   token[this].type!=T_END;   ++this) {
      if (token[this].type == T_COLON) {
         if (errchk  &&  token[this-1].hascolon)  ERROR (token[this].pos);
         token[this-1].hascolon = 1;
         delete_token (token, this);
         --this;
      }

      else if (token[this].type == T_DATA) {
         if (token[this-1].type == T_WORD) {
            chxcatval (token[this-1].str, THIN, L(' '));
            chxcat    (token[this-1].str, token[this].str);
            token[this-1].type = T_NAME;
         }
         else if (errchk)   ERROR (token[this-1].pos);
         delete_token (token, this);
         --this;
      }
   }

   RETURN  (1);
}
