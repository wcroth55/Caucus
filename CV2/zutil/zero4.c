/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/* ZERO4.   Write a file containing 4 0 bytes. */

/*: CP 11/30/93 13:21 1st carvmark. */

main (argc, argv)
   int   argc;
   char *argv[];
{
   static int4  zero = L(0);
   int   fd;

   fd = creat (argv[1], 0700);
   write (fd, &zero, 4);
   close (fd);
}
