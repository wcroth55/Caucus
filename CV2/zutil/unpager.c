
/*** UNPAGER.  Split up a collection of files created by PAGER.
/
/    UNPAGER reads a file created by PAGER, and writes each of the files
/    named in said input.  UNPAGER understands "standard pager format"
/    for file names, as well as native file system filename formats.
/    Standard pager format for a filename is:
/       [dir [dir...]] filename
/    where each word must be separated from the next by one space.
/
/    UNPAGER is invoked as either:
/
/       unpager <file.pag        (for systems that support input redirection)
/ or    unpager  file.pag        (for other systems)
/
/ */

/*** Copyright (C) 1990 Camber-Roth.   All rights reserved. */

/*: CR 10/28/89 12:17 Version 2.0. */
/*: CR 12/08/89 14:50 MSDOS: if strlen(name) > 8, insert a ".". */
/*: CR 12/xx/89 Primos filenames can't begin with a number. */
/*: CR  1/xx/90 AOS, Primos, VMS: "<" on command line is optional. */
/*: JV  9/11/90 Minor changes. */
/*: CR 09/18/90 11:30 Hack comments together. */
/*: CR  8/27/92 13:21 Remove all systype def's to typesys.h. */

/*** System type.   One of these must be defined as 1, or else
/    typesys.h must define one of them to be 1.  */
#define  UNIX     1
#define  VAXVMS   0
#define  MSDOS    0
#define  AOSVS    0
#define  PRIMOS   0
#define  CVM      0

#include <stdio.h>

#if UNIX | MSDOS | VAXVMS
#include <ctype.h>
#endif
#define  FUNCTION
#define  NOT     !

#define  LINEMAX  4096

FUNCTION  main (argc, argv)
   int   argc;
   char *argv[];
{
   char  line[LINEMAX], *name;
   FILE  *fo, *in, *fopen();
   int   first, semi, i;
   int   readstr();

#if VAXVMS | AOSVS | PRIMOS
   if (argc < 2) {
      printf ("Usage:  unpager file.pag\n");
      exit   (2);
   }
   
   name = argv[1];
   if (*name == '<')  ++name;

   if ( (in = fopen (name, "r")) == NULL) {
      printf ("Usage: Cannot open input file '%s'!\n", name);
      exit   (2);
   }
#endif

#if UNIX | MSDOS
   in = stdin;
#endif

   first = 1;
   while (fgets (line, LINEMAX, in) != NULL) {
      if (line[0]=='\032'  ||  line[1]=='\032')  continue;

      if (line[0]=='1'  &&  line[1]==' ') {
         for (i=strlen(line);   (NOT isprint(line[i-1]))  ||  line[i-1] == ' ';
               --i) ;
         line [i] = '\0';
         if (NOT first)  fclose (fo);

         for (name=line+1;   *name==' ';   ++name) ;
         makelocal (name);

         printf ("Opening %s...\n", name);
         if ( (fo = fopen (name, "w")) == NULL) {
            printf ("Failed at %s.\n", name);
            exit(2);
         }
         first = 0;
      }

      else if (line[0]==' ')  fprintf (fo, "%s", line+1);

      else                    fprintf (fo, "%s", line);
   }

   fclose (fo);

#if VAXVMS | AOSVS | PRIMOS
   fclose (in);
#endif
}

/*---MAKELOCAL converts the standard pager form of a filename
/    (dir dir ... file) into the local filename syntax. */
makelocal (name)
    char    name[];
{
    int     n, i, dot;
    char    newname[100], word[100];
    char   *w, *strtoken();

    /*** Count the number of words N. */
    for (n=0;   strtoken (word, n+1, name) != NULL;   ++n) ;
    newname[0] = '\0';

#if VAXVMS
    if (n > 1) {
       strcpy (newname, "[");
       for (i=0;   i<n-1;   ++i)  { 
          strtoken (word, i+1, name);
          strcat   (newname, ".");
          strcat   (newname, word);
       }
       strcat (newname, "]");
    }
    strtoken (word, n, name);
#endif

#if UNIX
    for (i=0;   i<n-1;   ++i) {
       strtoken (word, i+1, name);
       strcat   (newname, word);
       strcat   (newname, "/");
    }
    strtoken (word, n, name);
#endif

#if MSDOS
    for (i=0;   i<n-1;   ++i) {
       strtoken (word, i+1, name);
       strcat   (newname, word);
       strcat   (newname, "\\");
    }
    strtoken (word, n, name);

    if (strlen (word) > 8) {
       for (dot=0, w=word;   *w;   ++w)  if (*w=='.')  dot=1;
       if  (NOT dot) {
          strcpy (name, word);
          strcpy (word+9, name+8);
          word[8] = '.';
       }
    }
#endif

#if PRIMOS
    strcat (newname, "*>");
    for (i=0;   i<n-1;   ++i) {
       strtoken (word, i+1, name);
       strcat   (newname, word);
       strcat   (newname, ">");
    }
    strtoken (word, n, name);

    /*** Primos can not have a filename beginning with a digit! */
    if (isdigit(word[0])) {
       word[0] = 'r';
       strtoken (word+1, n, name);
    }
#endif

#if AOSVS
    for (i=0;   i<n-1;   ++i) {
       strtoken (word, i+1, name);
       strcat   (newname, word);
       strcat   (newname, ":");
    }
    strtoken (word, n, name);
#endif

    strcat   (newname, word);
    strcpy   (name, newname);
}


/*** STRTOKEN.  Find the POS'th token in a string.
/
/    STRTOKEN finds the POS'th token in the string LINE, and returns it
/    in WORD.  A token is a sequence of nonblank characters.  The first
/    token in a string is number 1.
/
/    STRTOKEN returns a pointer to the beginning of the found token in 
/    LINE.  If no such token was found, STRTOKEN returns NULL, and
/    sets WORD to the empty string. */

#define FALSE 0
#define TRUE  1

FUNCTION  char *strtoken (word, pos, line)
    char    word[], line[];
    int     pos;
{
    int   between, i, len;
    char  buf[100];
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
