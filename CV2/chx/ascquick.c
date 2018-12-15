
/*** ASCQUICK.  "Quick" conversion of chix to ascii.
/
/    a = ascquick (x);
/
/    Parameters:
/       char *a;       (ptr to buffer of regular ascii string)
/       Chix  x;       (chix to be converted)
/
/    Purpose:
/       Convert the contents of a chix into a regular "C" string of ascii
/       characters (an array of bytes, terminated by a 0 byte).
/   
/    How it works:
/       Ascquick() always returns a pointer to an internal static buffer.
/       The buffer contains up to the first 1000 characters from X,
/       converted to ascii by ascofchx().
/
/       If X is NULL, returns an empty string.
/
/       WARNING: Code maintainers: do not raise the limit on ascquick() 
/       above 1000 characters; other code that uses it depends on the
/       return being < 1000 chars in length to prevent buffer overflows.
/
/    Returns: pointer to char buffer.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  chx/ascquick.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/21/92 14:16 New function. */
/*: CR 11/15/92 18:14 Allow X to be NULL. */
/*: CR 12/10/92 17:11 Long ascofchx args. */
/*: CW  7/06/93 16:34 Expand ascquick result size to 1000 chars. */
/*: CR  4/04/00 12:57 Note in comments, don't raise result over 1000 chars! */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  char * ascquick (x)
   Chix   x;
{
   static char buf[1002];

   ENTRY ("ascquick", "");

   if (x == NULL  ||  ascofchx (buf, x, 0L, 1000L) < 0)  buf[0] = '\0';

   RETURN (buf);
}
