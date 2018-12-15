/*** LSTEST.   'ls'-like test program, to check Solaris variants
/
/    Invoked as:
/       lstest  path
/
/    Arguments:
/       path       pathname on disk to be listed
/
/    Purpose:
/       Lists files in 'pathname'.
/       Real purpose is to test minor variations in Solaris that
/       affect syspdir.c and sysgfdir.c.
/
/    How it works:
/       Calls sysgfdir().
/
/    Exit status:
/
/    Error Conditions:
/  
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  zutil/lstest.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/06/96 16:01 New program. */

#include <stdio.h>

main (argc, argv)
   int    argc;
   char  *argv[];
{
   int    start, error;
   char   fname[512];

   if (argc < 2) {
      printf ("Usage: lstest pathname\n");
      exit   (1);
   }

   for (start=1;   sysgfdir (argv[1], fname, start, &error) == 1;   start=0) {
      printf ("%s\n", fname);
   }
}
