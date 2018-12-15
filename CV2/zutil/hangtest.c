/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** HANGTEST.   Test program for Hangup signal detection. */

/*: CR  7/18/92 15:25 Carvmark the &*%$! thing! */

#include <stdio.h>
#include <signal.h>

int hangup = 0;

main (argc, argv)
   int   argc;
   char *argv[];
{
   FILE *fp, *fopen();
   char  line[102];
   int   syshang();

   if ( (fp = fopen ("hangtest.dat", "w")) == NULL) {
      printf ("Can't create hangtest.dat.\n");
      exit   (1);
   }

   fprintf (fp, "This is a test.\n");
   fclose  (fp);

   signal (SIGHUP,  syshang);
   signal (SIGQUIT, syshang);

   while (1) {
      printf (": ");
      fgets  (line, 100, stdin);
      printf ("%s\n", line);
      if (hangup)  break;
   }
}


syshang()
{
   hangup = 1;
   unlink ("hangtest.dat");
}
