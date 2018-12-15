
/*** ASCII_BUF.   Resizable string buffer.
/
/    a = ascii_buf (NULL, n);
/    ascii_buf (a, n);
/   
/    Parameters:
/       AsciiBuf   a;    (resizeable string buffer)
/       int        n;    (max size of buffer)
/
/    Purpose:
/       Create and upgrade resizeable string buffer(s).
/
/    How it works:
/       a = ascii_buf (NULL, n)
/          Creates a new ascii buffer, max size n.
/
/       ascii_buf (a, n)
/          Ensures that a is big enough to hold n chars.
/
/    Home:  sweb/asciibuf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/01/05 New function. */

#include <stdio.h>
#include "sweb.h"

FUNCTION  AsciiBuf ascii_buf (AsciiBuf a, int n) {
   char   *sysmem();
   char   *new;

   ENTRY ("ascii_buf", "");

   if (a == (AsciiBuf) NULL) {
      a = (AsciiBuf) sysmem (sizeof(struct asciibuf_t), "asciibuf");
      a->max = n;
      a->str = sysmem (n+1, "asciibufstr");
   }
   else if (a->max < n) {
      new = sysmem (n+1, "n+1");
      strcpy  (new, a->str);
      sysfree (a->str);
      a->str = new;
      a->max = n;
   }

   return (a);
}
