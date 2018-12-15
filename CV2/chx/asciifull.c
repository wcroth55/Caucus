
/*** ASCIIFULL.   "Full" conversion of chix to ascii.
/
/    c = asciifull (x);
/    ...
/    free (c);
/
/    Parameters:
/       char *c;      (returned, malloc'd ascii string)
/       Chix  x;      (chix to be converted)
/
/    Purpose:
/       Convert the contents of a chix into a regular "C" string of ascii
/       characters.
/   
/    How it works:
/       Asciifull() malloc's the necessary space for the ascii string,
/       so it is up to the caller to 'free()' it!!
/
/       If X is NULL, returns an empty string.
/
/    Returns: pointer to char buffer.
/
/    Known bugs:
/
/    Home:  chx/asciifull.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  6/11/03 New function. */

#include <stdio.h>
#include <stdlib.h>
#include "chixdef.h"

static char failed[12];

FUNCTION  char * asciifull (Chix x) {
   int    len, size;
   char  *p;
   char  *sysmem();

   ENTRY ("asciifull", "");

   if (x == NULL   ||  (len = chxlen (x)) <= 0)  {
      p = sysmem (12, "asciifull");
      if (p == NULL)  p = failed;
      p[0] = '\0';
      return (p);
   }

   size = chxtype(x) * len + 4;
   p    = sysmem (size, "asciifull size");
   if (p == NULL) {
      p = failed;
      p[0] = '\0';
      return (p);
   }

   if (ascofchx (p, x, 0L, size) < 0)  p[0] = '\0';

   RETURN (p);
}
