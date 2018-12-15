/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** addafter     Add text to a file *after* a particular line.
/
/    Usage:
/       addafter  filename  "text"  <addtext
/
/       Scans through 'filename' for "text" (must be quoted).
/       If found, adds text from stdin immediately after that line,
/       then continues adding the rest of the original text.
/
/       Only adds text from stdin once, after the *first* time "text"
/       is found.
/
/    CR 2004-03-28 New file.
/ */

#include <stdio.h>
#define  LINEMAX 2000

main (int argc, char *argv[]) {
   char  line[LINEMAX];
   char  outfile[300];
   int   state = 0;
   FILE *inp, *out, *fopen();

   if (argc < 2) {
      fprintf (stderr, "Usage: addafter file \"text\" <add\n");
      exit    (1);
   }

   inp = fopen (argv[1], "r");
   if (inp == NULL) {
      fprintf (stderr, "Can't open file '%s' for input.\n", argv[1]);
      exit    (2);
   }
   sprintf (outfile, "%s.tmp", argv[1]);
   out = fopen (outfile, "w");

   while (fgets (line, LINEMAX, inp) != NULL) {
      fprintf (out, "%s", line);

      if (state == 0  &&  strindex (line, argv[2]) >= 0) {
         while (fgets (line, LINEMAX, stdin))   fprintf (out, "%s", line);
         state = 1;
      }
   }
   fclose (inp);
   fclose (out);
   unlink (argv[1]);
   rename (outfile, argv[1]);

   exit   (0);
}

int strindex (s, t)
   char  s[], t[];
{
   register int  i, j, k;

   for (i=0;  s[i] != '\0';  i++) {
      for (j=i, k=0;  t[k] != '\0' && s[j]==t[k];  j++, k++) ;
      if (t[k] == '\0')  return (i);
   }
   return (-1);
}
