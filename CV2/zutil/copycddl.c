/*** copycddl.c  */

#include <stdio.h>

main (int argc, char *argv[]) {
   FILE *fp, *fopen();
   char  line[2000];
   int   state = 0;

   if ( (fp = fopen (argv[1], "r")) == NULL)  exit (1);

   while (fgets (line, 2000, fp) != NULL) {
      if (state == 0) {
         if (strindex (line, "#: CR")          == 0  ||
             strindex (line, "#: MH")          == 0  ||
             strindex (line, "#:CR")           == 0  ||
             strindex (line, "#-------------") == 0  ||
             line[0] == ' '  ||
             line[0] == '\n') {
            printf (
             "# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.\n"
             "# This program is published under the terms of the CDDL; see\n"
             "# the file license.txt in the same directory.\n"
             "#\n");
            state = 1;
         }
      }
      printf ("%s", line);
   }
}

/*** STRINDEX.  Return starting position in string S of string T. */
strindex (s, t)
   char  s[], t[];
{
   register int  i, j, k;

   for (i=0;  s[i] != '\0';  i++) {
      for (j=i, k=0;  t[k] != '\0' && s[j]==t[k];  j++, k++) ;
      if (t[k] == '\0')  return (i);
   }
   return (-1);
}
