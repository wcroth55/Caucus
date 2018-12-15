/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** LDAPTEST.   Simple program for testing ldap_verify on a server.
/
/    Invoked as:
/       ldaptest uid pw hostname portnum ssl/plain bindas
/
/ */

#include <stdio.h>

main (int argc, char *argv[]) {
   int   ok, port;
   char *opt;

   if (argc < 5) {
      printf ("Usage: ldaptest uid pw host port ssl/plain 'bindas'\n");
      exit   (1);
   }

   port = atoi (argv[4]);
   if (strcmp (argv[5], "ssl") == 0)  opt = "ssl";
   else                               opt = "";
   ok   = ldap_verify (argv[1], argv[2], argv[3], port, opt, argv[6], "", "");

   printf ("port=%d, ok=%d\n", port, ok);
}


#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define NOT   !

char *strtoken (word, pos, line)
   char    word[], line[];
   int     pos;
{
   int   between, i, len;
   char  buf[300];
   char  *w, *start;

   /*** BETWEEN means we are between tokens (in the white space).  */
   between  = TRUE;
   len = strlen (line);

   for (i=0;   i<=len;   i++) {

      if (line[i]==' '  ||  line[i]=='\0') {
         if (NOT between) {
            *w = '\0';
            if ((--pos) <= 0) { strcpy (word, buf);    return (start); }
         }
         between = TRUE;
      }

      else {
         if (between)  {
            w       = buf;
            start   = line + i;
            between = FALSE;
         }
         *w++ = line[i];
      }
   }

   word[0] = '\0';
   return (NULL);
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

streq (char *s, char *t) {
   return (strcmp (s, t) == 0);
}

logger (int i, int j, char *s) { }
