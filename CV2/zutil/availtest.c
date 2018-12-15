/*** AVAILTEST.   Report available disk space by testing sysavail().
/
/    Invoked as:
/       availtest   path
/
/    Arguments:
/       path       pathname on disk to be checked
/
/    Purpose:
/       Availtest reports the amount of free space (in bytes and
/       in number of file slots) on a disk.
/
/    How it works:
/       Calls sysavail().
/
/    Exit status: 0 normally.
/                 1 on bad usage.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  zutil/availtest.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CK  8/05/93 16:41 New program. */
/*: JV  9/26/93 13:49 Add debug. */

#include <stdio.h>
#include "int4.h"

main (argc, argv)
   int    argc;
   char  *argv[];
{
   int4   bytes, files;
   int    error;

   if (argc < 2) {
      printf ("Usage: availtest pathname\n");
      exit   (1);
   }

   if (sysavail (argv[1], &bytes, &files, &error)) 
      printf ("kbytes=%d\nfiles=%d\n", bytes, files);
   else
      printf ("error=%d\n", error);
}
