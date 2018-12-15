
/*** MDFINDER.  Filter to find Caucus dictionary string names in source files.
/
/    Invoked as:
/       mdfinder <source_file >result
/
/    Arguments:
/       source_file   name of Caucus source code file
/       result        name of output file containing string names
/
/    Purpose:
/       This program scans a Caucus source code file and attempts to find
/       any and all references to dictionary string names.
/
/    How it works:
/       Mdfinder ignores everything in comments.
/
/       Otherwise, it looks for double-quoted strings, and writes each
/       such string to stdout, one per line.  Following each string is
/       at least one blank, followed by the original line of code up to
/       the point where the string was found.
/
/    Exit status: 0
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
/    Home:  mdfinder.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/19/93 11:33 New program. */

#include <stdio.h>

#define   FUNCTION
#define   NOT         !
#define   APPLINE(x)  line[lptr++] = x;  line[lptr] = '\0'
#define   APPSTR(x)   qstr[qptr++] = x;  qstr[qptr] = '\0'

#define   CODE     0
#define   OPENCOM  1
#define   COMMENT  2
#define   CLOSECOM 3
#define   QUOTED   4

FUNCTION  main (argc, argv)
   int    argc;
   char  *argv[];
{
   int    c, state, isdic;
   char   line[1000];
   int    lptr;
   char   qstr[1000];
   int    qptr;
   char  *blank;

   state = CODE;
   lptr  = 0;
   qptr  = 0;
   isdic = 1;

   while ( (c = getchar()) != EOF)  switch (state) {

      case (CODE):
         if (c == '/')  { state = OPENCOM;                              break; }
         if (c == '\n') { lptr  = 0;                                    break; }
         if (c == '"')  { state = QUOTED;    isdic = 1;   APPLINE(c);   break; }
         APPLINE(c);
         break;

      case (OPENCOM):
         if (c == '*')  { state = COMMENT;   break; }
         state = CODE;
         APPLINE('/');
         APPLINE(c);
         break;

      case (COMMENT):
         if (c == '*')  state = CLOSECOM;
         break;

      case (CLOSECOM):
         if (c == '/')  state = CODE;
         else           state = COMMENT;
         break;

      case (QUOTED):
         APPLINE(c);
         if (c == ' ')    isdic = 0;
         if (c == '\n')   isdic = 0;
         if (c != '"')  { APPSTR(c);      break; }

         if (isdic) {
            for (blank=line;   *blank==' ';   ++blank) ;
            printf ("%-20s %s\n", qstr, blank);
         }
         qptr = 0;
         qstr[0] = '\0';
         state = CODE;
         break;
   }
}
