
/*** CHOPPER.C.   Chop a file into smaller pieces.
/
/    Invoked as:
/       chopper file kbyte
/
/    Arguments:
/       file       file name
/       kbyte      number of k-byte segments
/
/    Purpose:
/       Chopper takes a large file and breaks it up into successive
/       segments that are KBYTE sets of 1024K byte blocks.  This is
/       useful, for example, for breaking large 'tar' files into
/       smaller sized pieces that can be placed on small media, 
/       such as floppy disks.
/
/       Files broken up with chopper can be simply reassembled with 'cat'.
/
/    How it works:
/       The first piece is written to FILE.000, the second piece to
/       FILE.001, etc.
/
/       The last piece may be smaller than KBYTE kbytes.
/
/    Exit status: 0 normally
/                 1 if bad usage
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
/    Home:  chopper.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/20/92 20:29 New program. */

#include <stdio.h>
#include <fcntl.h>

main (argc, argv)
   int    argc;
   char  *argv[];
{
   char   piece[100], data[1024];
   int    chunks, fd, pc, length, this, i;

   if (argc < 3) {
      printf ("Usage: chopper file kbytes\n");
      exit   (1);
   }

   sscanf (argv[2], "%d", &chunks);

   if ( (fd = open (argv[1], O_RDONLY)) < 0) {
      printf ("Can't open file '%s'.\n", argv[1]);
      exit   (1);
   }

   for (length=1024, this=0;   length>0;   ++this) {

      sprintf (piece, "%s.%03d", argv[1], this);
      if ( (pc = open (piece, O_WRONLY | O_CREAT, 0755)) < 0) {
         printf ("Error!  Can't write to '%s'.\n", piece);
         exit   (1);
      }

      for (i=0;   i<chunks;   ++i) {
         if ( (length = read (fd, data, 1024)) <= 0)  break;
         write (pc, data, length);
      }

      close (pc);
   }

   close (fd);
   exit  (0);
}
