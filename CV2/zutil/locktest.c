/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** LOCKTEST.   Test ranges of numbers that may be used by lockf() & lseek().
/
/    Invoked as:
/       locktest   byte_number
/ */

/*: CR  9/05/91 16:55 Initial version. */
/*: CR  4/11/03 Use <errno.h> */
/*: CR 11/12/03 Use SEEK_SET in lseek() call. */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

main (argc, argv)
   int   argc;
   char *argv[];
{
   int4  bnum;
   int   fd;
   char  junk[80];
   int4  lseek();

   if (argc < 2) {
      printf ("Usage: locktest byte_number\n");
      exit   (1);
   }

   sscanf (argv[1], "%d", &bnum);

   if ( (fd = creat ("locktest.dat", 0750)) < 0  &&
        (fd = open  ("locktest.dat", 2))    < 0) {
      printf ("Can't create locktest.dat!\n");
      exit   (1);
   }

   if (lseek (fd, bnum, SEEK_SET) < L(0)) {
      printf ("Can't seek to %d.\n", bnum);
      perror ("");
      printf ("errno=%d\n", errno);
      close  (fd);
      exit   (1);
   }

   if (lockf (fd, F_TLOCK, L(1)) < 0) {
      printf ("Can't lock byte %d\n", bnum);
      close  (fd);
      exit   (0);
   }

   printf ("Byte %d is locked.  Press RETURN to unlock.\n", bnum);
   gets   (junk);

   lockf (fd, F_ULOCK, L(1));
   close (fd);
}
