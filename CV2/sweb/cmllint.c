/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** CMLLINT.   Primitive tester for CML bugs. */

#include <stdio.h>

int debug = 0;

main (argc, argv)
   int   argc;
   char *argv[];
{
   FILE *fp, *fopen();
   char  line[200], word[100];
   int   endcount, pos;

   if (argc < 2) {
      fprintf (stderr, "Usage: cmllint filename\n");
      exit    (1);
   }

   if ( (fp = fopen (argv[1], "r")) == NULL) {
      fprintf (stderr, "Can't open file '%s'.\n", argv[1]);
      exit    (1);
   }

   endcount = 0;
   while (fgets (line, 200, fp) != NULL) {
      line [strlen(line)-1] = '\0';
      for (pos=0;   line[pos];   ++pos)
         if (line[pos] == '\t')  line[pos] = ' ';

      strtoken (word, 1, line);
      if (streq (word, "for")  ||  streq (word, "count")  ||
          streq (word, "if")   ||  streq (word, "else"))      ++endcount;

      if (streq (word, "end"))  --endcount;
   }

   if (endcount > 0)  printf ("There are %d missing end(s).\n", endcount);
   if (endcount < 0)  printf ("There are %d too many end(s).\n", -endcount);

   fclose (fp);
}
