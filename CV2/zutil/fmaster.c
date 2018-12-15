
/*** FMASTER.  Fix Caucus XCMA "masteres" file.
/
/    Invoked as:
/       ls [0-9A-Z]......... >infile
/       fmaster    infile   masteres
/
/    Arguments:
/       infile     name of file containing list of item/resp partfiles
/       outfile    new 'masteres' file for this conference
/
/    Purpose:
/       For reasons unknown, on occasion the contents of the XCMA
/       "masteres" file may get trashed.  Most of the information in
/       this file can be extracted from the XCRF response files, and
/       that's exactly what FMASTER does.
/
/    How it works:
/       Fmaster reads through all of the response partfiles, and
/       writes a new 'masteres' file.  It must be run in the
/       directory of the offending conference (e.g., in C005.)
/
/       Fmaster prompts the user for the userid of the organizer.
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
/    Home:  zutil/fmaster.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/18/92 15:25 Carvmark the &*%$! thing! */
/*: CR  2/02/93 11:41 Expand to handle 10,000 items, add header. */
/*: CR  2/08/93 15:32 Declare str36toi(). */

#include <stdio.h>
#include <ctype.h>
#include "int4.h"
#define  NOT  !

#define  MAXITEM  9950
short responses[MAXITEM];

main (argc, argv)
   int   argc;
   char *argv[];
{
   FILE *in, *ot, *fp, *fopen();
   int   item, rnum, i;
   char  respfile[200], line[200], userid[60];

   if (argc < 3) {
      printf ("Usage:  FMASTER infile outfile\n");
      exit   (1);
   }

   if ( (in = fopen (argv[1], "r")) == NULL) {
      printf ("Can't open infile '%s'\n", argv[1]);
      exit   (1);
   }

   nt_mkfile (argv[2], 0700);
   if ( (ot = fopen (argv[2], "w")) == NULL) {
      printf ("Can't open outfile '%s'\n", argv[2]);
      exit   (1);
   }

   /*** Initialize table of items to mark each item deleted. */
   for (item=1;   item<MAXITEM;   ++item)  responses[item] = -1;

   while (fgets (respfile, 200, in) != NULL) {
      trim (respfile);
      item = itemnumber (respfile);

      /*** Find highest response number in this file. */
      if ( (fp = fopen (respfile, "r")) == NULL)  continue;
      while (fgets (line, 200, fp) != NULL) {
         if (line[0]!='#')  continue;
         sscanf (line+1, "%d", &rnum);
         if (rnum > responses[item])  responses[item] = rnum;
      }
      fclose (fp);

   }
   fclose (in);


   /*** Finally, prompt for the userid, and write out the new
   /    masteres file. */
   printf ("Enter userid of organizer: ");   fflush (stdout);
   fgets  (userid, 40, stdin);
   fprintf (ot, "%s%d 0 0\n", userid, item);
   for (i=1;   i<=item;   ++i)  fprintf (ot, "%d\n", responses[i]);
   fclose (ot);
}

/*----------------------------------------------------------*/

trim (filename)
   char *filename;
{
   char *s;

   for (s=filename;   *s!=' '  &&  *s!='\n'  &&  *s!='\0';   ++s) ;
   *s = '\0';
}

/*----------------------------------------------------------*/

itemnumber (f)
   char  *f;
{
   int   item;
   char  temp[200];
   int4  str36toi();

   strcpy (temp, f);
   temp[3] = '\0';
   if (isdigit(temp[0]))  sscanf (temp, "%3d", &item);
   else                   item = str36toi (temp) - 11960;
   return (item);
}
