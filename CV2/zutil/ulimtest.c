
/*** ULIMTEST.   Test ulimit, apply to a command line.
/
/    Invoked as:
/       ulimtest  size  command
/
/    Arguments:
/       size       max number of blocks process may use in creating new files
/       command    a Unix command to run, with SIZE enforced.
/
/    Purpose:
/       ULIMTEST is meant as a tool to test what happens to a Unix command
/       when a process fails because it has run out of disk space while
/       writing a file.
/
/    How it works:
/       Calls ulimit() to set the new maximum to SIZE.  Then spawns off
/       a Bourne shell to run COMMAND.
/
/    Exit status:
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
/    Home:  zutil/ulimtest.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/16/91 15:54 New program. */

#include <stdio.h>

main (argc, argv)
   int   argc;
   char *argv[];
{
   int4  size, was, ulimit();
   char  line[400];
   int   i;

   if (argc < 3) {
      printf ("Usage: ulimtest size command\n");
      exit   (1);
   }

   sscanf (argv[1], "%d", &size);
   was = ulimit (1, size);
   ulimit (2, size);
   printf ("Old limit was %d, new limit is %d\n", was, size);

   for (line[0]='\0', i=2;    i<argc;    ++i) {
      strcat (line, argv[i]);
      strcat (line, " ");
   }
   system (line);
}
