/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** RUNSQL.    Run arbitrary SQL statements via ODBC.
/
/    Invoked (from cron or $xshell()) as:
/       runsql swebd.conf <input
/
/ */

/*: CR  5/23/03 New program. */
/*: CR  7/23/03 Look for ; at end of lines */

#include <stdio.h>
#include "sqlcaucus.h"
#define  FUNCTION
#define  EMPTY(x)  (!x[0])

FUNCTION int main (int argc, char *argv[]) {
   int   rc;
   FILE *swebd;
   char  line[2000], word[200], string[10000];
   char  connect[200];
   char  odbcini[200];
   char  odbcsysini[200];
   char  path[200];
   int   len;

   if (argc < 2  ||  (swebd = fopen (argv[1], "r")) == NULL) {
      fprintf (stderr, "Usage: runsql swebd.conf\n");
      exit (1);
   }

   /*** Parse needed parameters from swebd.conf. */
   connect[0] = odbcini[0] = odbcsysini[0] = path[0] = '\0';
   while (fgets (line, 200, swebd)) {
      line [strlen(line)-1] = '\0';
      strtoken (word, 1, line);
      strlower (word);
      if (strindex (word, "connectstring") == 0) strtoken (connect,    2, line);
      if (strindex (word, "odbcini")       == 0) strtoken (odbcini,    2, line);
      if (strindex (word, "odbcsysini")    == 0) strtoken (odbcsysini, 2, line);
      if (strindex (word, "caucus_path")   == 0) strtoken (path,       2, line);
   }
   fclose (swebd);

   /*** Connect to database. */
   rc = sql_init (connect, odbcini, odbcsysini);
   if (rc != SQLCAU_OK)
      printf ("%s: rc=%8d, connect: connect=%s, odbcini=%s, odbcsysini=%s, "
              "path=%s\n", mytime(), rc, connect, odbcini, odbcsysini, path);

   string[0] = '\0';
   while (fgets (line, 2000, stdin) != NULL) {
      len = strlen (line) - 1;
      if (line[len] == '\n')  line[len--] = '\0';
      while (line[len] == ' ')     len--;
      if    (line[len] == ';') {
         line[len] = '\0';
         strcat (string, line);
         rc = sql_exec (string);
         if (rc != SQLCAU_OK) 
            printf ("%s: rc=%8d, runsql: '%s'\n", mytime(), rc, line);
         string[0] = '\0';
      }
      else strcat (string, line);
   }

   if (string[0]) {
      rc = sql_exec (string);
      if (rc != SQLCAU_OK) 
         printf ("%s: rc=%8d, runsql: '%s'\n", mytime(), rc, line);
   }

   sql_done();
}
