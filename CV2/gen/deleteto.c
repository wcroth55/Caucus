
/*** DELETE_TOKEN.   Delete a token in an array of parsing tokens.
/
/    delete_token (token, this);
/
/    Parameters:
/       Token  token[];     (array of tokens, supplied by caller)
/       int    this;        (index to token to be deleted)
/
/    Purpose:
/       Delete a particular token in an array of command-line parsing
/       tokens.
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
/    Called by:  new parsing functions
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  gen/deleteto.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  6/13/93 22:41 New function. */

#include <stdio.h>
#include "caucus.h"
#include "token.h"

FUNCTION  delete_token (token, this)
   Token  token[];
   int    this;
{
   ENTRY ("delete_token", "");

   if (token[this].str != nullchix)  chxfree (token[this].str);
   for (;   token[this].type != T_END;   ++this)
      token[this] = token[this+1];

   RETURN  (1);
}
