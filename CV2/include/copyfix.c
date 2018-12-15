#include <stdio.h>

main (int argc, char *argv[]) {
   FILE *fp, *fopen();
   char  line[2000];
   int   s, c, found;

   if ( (fp = fopen (argv[1], "r")) == NULL)  exit (1);

   /*** Does it have a Copyright notice? */
   found = 0;
   while (fgets (line, 2000, fp) != NULL) {
      s = strindex (line, "/*");
      c = strindex (line, " Copyright ");
      if (s >= 0  &&  c > s) {  found = 1;   break; }
   }
   fclose (fp);


   if (found == 0)  gpl();
   
   fp = fopen (argv[1], "r");
   while (fgets (line, 2000, fp) != NULL) {
      s = strindex (line, "/*");
      c = strindex (line, " Copyright ");
      if (s >= 0  &&  c > s)  { gpl(); break; }
      printf ("%s", line);
   }
   while (fgets (line, 2000, fp) != NULL)   printf ("%s", line);
}

gpl() {
   printf (
      "/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.\n"
      "/    This program is published under the terms of the GPL; see\n"
      "/    the file license.txt in the same directory.  */\n");
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
