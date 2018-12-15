
/*** MERGE2.   Merge two files with minor differences.
/
/    Invoked as:
/       merge2  file1  file2   output   [spread]
/
/    Arguments:
/       file1      name of 1st input file
/       file2      name of 2nd input file
/       output     name of output file
/       spread     number of lines of spread
/
/    Purpose:
/       MERGE2 attempts to "merge" 'file1' and 'file2' into 'output'.  Each
/       line in 'output' begins with a blank, the digit 1, or the digit 2.
/
/       Blank means the line is common to both files.
/       "1"   means the line is only in file1.
/       "2"   means the line is only in file2.
/
/    How it works:
/       MERGE2 loads both files entirely into heap memory, and then attempts
/       to scan down through them, looking for "blocks" of lines that are
/       identical.  SPREAD controls how far forward and back in the file
/       (as a number of lines) MERGE2 looks; the default value is 200.
/
/       For the purpose of considering two lines identical, MERGE2 ignores
/       any leading blanks or tabs, or any trailing blanks, on a line.
/
/    Exit status: 0 normally.
/                 2 on bad usage or input files too big.
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
/    Home:  merge2.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved. */

/*** Merge2 is distributed as an "open source" tool under the terms
/    of the MIT artistic license (see www.opensource.org for details).
/    Basically, you can use or distribute this code for any purpose, free of 
/    charge, so long as you retain all of this notice.  This is an "as-is"
/    tool; no warranties, implicit or explicit, are provided.  The author is
/    not liable for any damages or effects resulting from the use of
/    this tool.  */

/*: CR  7/24/91 20:38 Carvmark the thing! */
/*: CR  9/11/92 14:37 Raise MAX to 4500.  */
/*: CR 10/05/92 11:47 Ignore leading and trailing spaces on lines. */
/*: CK  9/16/93 22:14 Raise MAX to 6000.  */
/*: CR  8/31/94 11:58 Raise max line length to 2000. */
/*: CR  2/09/07 Treat leading tabs same as blanks.  Remove trailing 
/               carriage returns, aka '\r'. */

#include <stdio.h>
#define  FUNCTION
#define  NOT   !
#define  MAXROW 200000    /* Maximum number of lines of text. */
#define  MAXLEN   2000    /* Maximum size of a line of text. */

struct line_t {
   char  *s;     /* Pointer to line of text. */
   short  lnk;   /* Link to line in "other" file, or -1 if no link. */
   short  size;  /* Size of block of matches this line belongs to.  */
};

struct line_t a[MAXROW], b[MAXROW];

FUNCTION  main (argc, argv)
   int   argc;
   char *argv[];
{
   FILE *fa, *fb, *out, *fopen();
   int   alines, blines, spread, i, i0, j, j0, j1, state, output, match;
   int   ai, bi, size;
   char *str;

   if (argc < 4)  die ("Usage: MERGE2 filea fileb output [spread]", "");

   if ( (fa  = fopen (argv[1], "r")) == NULL  ||
        (fb  = fopen (argv[2], "r")) == NULL  ||
        (out = fopen (argv[3], "w")) == NULL) 
        die ("Can't open one of the files.", "");

   if (argc == 5)  sscanf (argv[4], "%d", &spread);
   else            spread = 200;

   /*** Load the files into memory. */
   alines = loadfile (fa, a);
   fclose (fa);
   if (alines == -1)  die ("Too many lines in file ", argv[1]);
   if (alines == -2)  die ("Out of memory in file ",  argv[1]);

   blines = loadfile (fb, b);
   fclose (fb);
   if (blines == -1)  die ("Too many lines in file ", argv[2]);
   if (blines == -2)  die ("Out of memory in file ",  argv[2]);

   /*** Find exact matches between pairs of lines in the files. */
   for (i=0;   i<alines;   ++i) {
      str = a[i].s;
      j0 = (i-spread >= 0      ? i-spread : 0);
      j1 = (i+spread <  blines ? i+spread : blines);
      for (j=j0;   j<j1;   ++j)   if (same (str, b[j].s)) {
         if (a[i].lnk >= 0)  { a[i].lnk = -1;   break; }
         a[i].lnk = j;
      }
      if (a[i].lnk >= 0)  b[a[i].lnk].lnk = i;
   }


   /*** Start at the top, and look for growth of matches downwards. 
   /    If A[i-1] matches B[j-1], then look at A[i] and B[j].  If they 
   /    are the same, we'll force this to be a match. */
   for (i=1;   i<alines;   ++i) {
      if (a[i-1].lnk <  0)  continue;
      if (a[i  ].lnk >= 0)  continue;
      j = a[i-1].lnk + 1;
      if (j >= blines)      continue;
      if (b[j  ].lnk >= 0)  continue;
      if (NOT same (a[i].s, b[j].s))  continue;
      a[i].lnk = j;
      b[j].lnk = i;
   }


   /*** Start at the bottom, and look for growth of matches upwards. 
   /    If A[i+1] matches B[j+1], then look at A[i] and B[j].  If they 
   /    are the same, we'll force this to be a match. */
   for (i=alines-2;   i>=0;   --i) {
      if (a[i+1].lnk <  0)  continue;
      if (a[i  ].lnk >= 0)  continue;
      j = a[i+1].lnk - 1;
      if (j < 0)            continue;
      if (b[j  ].lnk >= 0)  continue;
      if (NOT same (a[i].s, b[j].s))  continue;
      a[i].lnk = j;
      b[j].lnk = i;
   }


   /*** Find blocks of exact matches, and figure their sizes. 
   /    State=0 means not in a match, 1 means in middle of a match. */
   state = 0;
   for (i=0;   i<alines;   ++i) {
      /*** This line is a match if the lines are identical, and either
      /    (1) the previous lines in A and B had no link, or (2) were
      /    linked to each other. */
      match = 0;
      if ( (j=a[i].lnk) >= 0) {
         if (i==0  ||  j==0)  match = 1;
         else                 match = (a[i-1].lnk == j-1);
      }

      /*** If we're starting a block, record its starting line. */
      if (NOT state  &&  match) {
         i0 = i;
         state = 1;
      }

      /*** If ending a block of matches, compute and fill in size elements
      /    for all members of the block in both A and B. */
      else if (state  &&  NOT match) {
         size = i - i0;
         for (j=i0;          j<i;             ++j)  a[j].size = size;
         for (j=a[i0].lnk;   j<=a[i-1].lnk;   ++j)  b[j].size = size;
         state = 0;
      }
   }
   /*** End of file is also end of a block, if we're in one. */
   if (state) {
      size = i - i0;
      for (j=i0;          j<i;             ++j)  a[j].size = size;
      for (j=a[i0].lnk;   j<=a[i-1].lnk;   ++j)  b[j].size = size;
   }
      

   /*** Now for the grand finale.  Skip through A and B, and write
   /    out the blocks and lines as appropriate. */
   ai = bi = 0;
   while (ai < alines  ||  bi < blines) {
      /*** If one file is at the end, all that's left is to write the other. */
      if      (ai >= alines)  output = 2;
      else if (bi >= blines)  output = 1;

      /*** If a match, print it as such. */
      else if (same (a[ai].s, b[bi].s))   output = 0;

      /*** If not a match, print this file's contribution. */
      else if (a[ai].lnk < 0)  output = 1;
      else if (b[bi].lnk < 0)  output = 2;

      /*** If we matched an already passed line, this is the final part of
      /    an "X" crossing. */
      else if (a[ai].lnk < bi) output = 1;
      else if (b[bi].lnk < ai) output = 2;

      /*** The only output remaining (I hope!) is the top part of an "X"
      /    crossing.  In that case, choose the smaller block and print it
      /    first, hoping to have only one copy of the larger block. */
      else if (a[ai].size < b[bi].size)  output = 1;
      else                               output = 2;

      if (output==1) { fprintf (out, "1%s\n", a[ai].s);   ++ai;       }
      if (output==2) { fprintf (out, "2%s\n", b[bi].s);   ++bi;       }
      if (output==0) { fprintf (out, " %s\n", b[bi].s);   ++ai;  ++bi; }
   }
   fclose (out);

   exit (0);
}


/*----LOADFILE.   Load a file into memory. ------------------------- */
FUNCTION  loadfile (fp, line)
   FILE  *fp;
   struct line_t line[];
{
   char   buf[MAXLEN];
   int    i, len;
   char  *malloc();

   for (i=0;   fgets (buf, MAXLEN-2, fp) != NULL;   ++i) {
      len = strlen (buf) - 1;
      if    (len>0  &&  buf[len-1]=='\r')  --len;
      while (len>2  &&  buf[len-1]==' ' )  --len;
      buf[len] = '\0';
      if (i >= MAXROW-1)      return (-1);
      line[i].s = malloc (len+2);
      if (line[i].s == NULL)  return (-2);
      strcpy (line[i].s, buf);
      line[i].lnk  = -1;
      line[i].size =  0;
   }

   return (i);
}

/*---DIE.   Exit with an error message. ------------------------- */
FUNCTION  die (s1, s2)
   char  *s1, *s2;
{
   printf ("%s%s\n", s1, s2);
   exit   (2);
}

/*---SAME.  Test similarity of two strings. ------------------------- */
FUNCTION  same (s, t)
   char *s, *t;
{
   while (*s == ' ' || *s == '\t')  ++s;
   while (*t == ' ' || *t == '\t')  ++t;
   while (*s == *t)  {
      if (NOT *s)  return (1);
      ++s;
      ++t;
   }
   return (0);
}
