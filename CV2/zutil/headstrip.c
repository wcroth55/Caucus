
/*** HEADSTRIP.   Strip off "Jeff headers" from C-R source code, assemble.
/
/    Invoked as:
/       headstrip  <list   >output
/
/    Arguments:
/       list       list of input source files.
/       output     resulting output
/
/    Purpose:
/       Produce a listing of the header comments only of a set of
/       C source code files.
/
/    How it works:
/       Headstrip reads the initial header comments from each file
/       mentioned in LIST.  The comments are written to stdout, 
/       separated by ^L's.
/
/    Exit status:
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
/    Home:  zutil/headstrip.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/02/92 14:21 New program. */

#include <stdio.h>

#define  NOT         !
#define  FUNCTION

FUNCTION  main (argc, argv)
   int    argc;
   char  *argv[];
{
   FILE  *fp, *fopen();
   int    comment;
   char   line[202], name[202];

   while (fgets (name, 200, stdin) != NULL) {
      name [ strlen(name)-1 ] = '\0';

      if ( (fp = fopen (name, "r")) == NULL) {
         fprintf (stderr, "Can't open file '%s'.\n", name);
         continue;
      }

      for (comment=0;   fgets (line, 200, fp) != NULL;   ) {
         if (NOT comment  &&  line[0]!='/')  continue;
         if (    comment  &&  line[0]!='/')  break;
         comment = 1;
         printf ("%s", line);
      }
      fclose (fp);
      printf ("%\014\n");
   }
}
