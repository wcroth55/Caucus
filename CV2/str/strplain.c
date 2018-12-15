
/*** STRPLAIN.   En/decrypt a license block string.
/
/    result = strplain (str);
/
/    Parameters:
/       char  *result;      (ptr to static string containing result)
/       char  *str;         (string to be en/decrypted)
/
/    Purpose:
/       The Caucus license block contains several strings which are
/       encrypted so that the customer may not find and alter the
/       license information.  Strplain is used to encrypt and decrypt
/       such strings.
/
/    How it works:
/       Strplain() xor's each byte of STR against a standard mask, and
/       places the result in a static block contained in itself.  It
/       returns the pointer to that block.  Thus, the results of
/       strplain() are ephemeral, and must be used immediately.
/
/    Returns: pointer to internal static block.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  str/strplain.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/26/93 15:06 New function. */

#include <stdio.h>
#include "caucus.h"
#include "plain.h"

FUNCTION  char *strplain (str)
   char  *str;
{
   static char result[32];
   char  *s, *p, *r;

   for (s=str, p=PLAIN_ENC_STR, r=result;   *s && *p;   ++s, ++p, ++r)
      *r = *s ^ *p;

   *r = '\0';
   return (result);
}
