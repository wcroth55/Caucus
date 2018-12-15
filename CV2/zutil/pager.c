
/*** PAGER.  "Page" the contents of a list of files together.
/
/    Invoked as:
/       pager <input  >output    (unix, msdos)
/ or    pager  input   output    (vax/vms, primos)
/
/    Arguments:
/       input      name of file containing list of files to be pager'd.
/       output     name of file to contain pager'd-together data.
/
/    Purpose:
/       Archive a set of files together into one file.
/
/    How it works:
/       PAGER reads a list of file names, and writes the contents of all
/       of the files to an output file.
/   
/       Each file written into the output file begins with a "1" in
/       column 1, followed by a blank and the name of the file.  The
/       remaining lines of the file are written prefixed by a blank.
/   
/       The input list of files must contain one file name per line.
/       Each file name may be in the native file system format, or
/       in "pager standard format", which is:
/   
/          [dir [dir ...]] filename
/   
/       where each word is separated by one or more spaces.  Directory
/       references in pager standard format are always assumed to be
/       relative to the current directory.  This means that spaces are
/       not allowed in filenames.
/
/       Implemented for: Unix, Vax/VMS, AOS/VS, PRIMOS, MSDOS.
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
/    Known bugs:      none
/
/    Notes:
/       (The next thing to work on is to force PAGER to take native
/        file names and always translate them into pager standard form.)
/
/    Home:  carves/pager.c
/ */

/*** Copyright (C) 1990 Camber-Roth.  All rights reserved. */

/*: JV 09/11/90 00:00 Add "pager standard format". */
/*: CR 09/18/90 12:19 Add filename arg checking, MSDOS & PRIMOS.  */
/*: CR 10/30/90 19:26 Fix bug: 1-word filenames aren't picked up! */
/*: CR  8/27/92 13:21 Remove all systype def's to typesys.h. */
/*: CR  6/01/93 15:40 Add new header comments. */
/*: CR  4/05/01 11:40 Force all output lines to end in '\n'. */

#define  UNIX     1
#define  VAXVMS   0
#define  MSDOS    0
#define  AOSVS    0
#define  PRIMOS   0
#define  CVM      0

#include <stdio.h>
#define FUNCTION
#define NOT     !

#define LINEMAX 4096

FUNCTION  main (argc, argv)
   int    argc;
   char  *argv[];
{
   FILE  *fp, *in, *out, *fopen();
   char   file[200], line[LINEMAX], *s, fileparts[200], dummy[200], *strtoken();
   char  *name;
   short  filepartnum, i, slen;

#if VAXVMS | AOSVS | PRIMOS
   if (argc < 3) {
      printf ("Usage:  unpager input_list output\n");
      exit   (2);
   }

   name = argv[1];
   if (*name == '<')  ++name;
   if ( (in = fopen (name, "r")) == NULL) {
      printf ("Usage: Cannot open input file '%s'!\n", name);
      exit   (2);
   }

   name = argv[2];
   if (*name == '>')  ++name;
   if ( (out = fopen (name, "w")) == NULL) {
      printf ("Usage: Cannot open output file '%s'!\n", name);
      exit   (2);
   }
#endif

#if UNIX | MSDOS
   in  = stdin;
   out = stdout;
#endif

   while (fgets (file, 200, in) != NULL) {
      slen = strlen(file);
      if (file[slen-1]=='\n'  ||  file[slen-1]=='\r')  --slen;
      if (file[slen-1]=='\n'  ||  file[slen-1]=='\r')  --slen;
      file[slen] = '\0';

      strsimplify (file);                           /* extra internal blanks */
      strcpy      (fileparts, file);

      for (filepartnum=1, s=file;   *s;   s++)
         if (*s == ' ') filepartnum++;     /* Count number of pathname parts. */
                     /* The last part is the filename, others are dir parts. */

      /*** If multiple parts, we need to massage the filename according
      /    to the requirements of the file system. */
      if (filepartnum > 1) {

#if UNIX
         file[0]='\0';
         for (i=1; i<=filepartnum; i++) {             /* Copy each part over */
            strtoken (dummy, i, fileparts);
            strcat (file, dummy);
            if (i != filepartnum)            /* Add separators if necessary */
               strcat (file, "/");
         }
#endif

#if MSDOS
         file[0]='\0';
         for (i=1; i<=filepartnum; i++) {             /* Copy each part over */
            strtoken (dummy, i, fileparts);
            strcat (file, dummy);
            if (i != filepartnum)            /* Add separators if necessary */
               strcat (file, "\\");
         }
#endif

#if PRIMOS
         strcpy (file, "*>");
         for (i=1; i<=filepartnum; i++) {             /* Copy each part over */
            strtoken (dummy, i, fileparts);
            strcat (file, dummy);
            if (i != filepartnum)            /* Add separators if necessary */
               strcat (file, ">");
         }
#endif

#if AOSVS
         file[0]='\0';
         for (i=1; i<=filepartnum; i++) {             /* Copy each part over */
            strtoken (dummy, i, fileparts);
            strcat (file, dummy);
            if (i != filepartnum)             /* Add separators if necessary */
               strcat (file, ":");
         }
#endif

#if VAXVMS
         strcpy (file, "[.");                   /* Start with stupid VMS box. */
         for (i=1; i<=filepartnum-1; i++) {        /* Copy each dir part over */
            strtoken (dummy, i, fileparts);
            strcat (file, dummy);
            if (i != filepartnum-1)           /* Add separators if necessary */
               strcat (file, ".");
         }
                          /* Tack on another stupid VMS box and the filename. */
         strcat (file, "]");
         strcat (file, strtoken (dummy, filepartnum, fileparts));
#endif
      }

      if ((fp = fopen (file, "r")) == NULL)
         fprintf (stderr,
              "Could not open file %s, proceeding with other files\n", file);
      else {
         fprintf (stderr, "%s\n", file);
         fprintf (out, "1 %s\n", fileparts);

         while   (fgets (line, LINEMAX, fp) != NULL) {
            fprintf (out, " %s", line);
            if (line[strlen(line)-1] != '\n')  fprintf (out, "\n");
         }
         fclose  (fp);
      }
   }

#if VAXVMS | AOSVS | PRIMOS
   fclose (in);
   fclose (out);
#endif
}


/*** STRSIMPLIFY.  Simplify a string by removing all unnecessary blanks.
/
/    Specifically, replace all multiple blanks with one blank,
/    and remove all leading and trailing blanks. */

FUNCTION  int strsimplify (str)
   char  str[];
{
   int   len;

   for (len=strlen(str);   len>0  &&  str[len-1]==' ';)  --len;
   str[len] = '\0';

   while (str[0] == ' ')  stralter (str, " ", "");

   while (stralter (str, "  ", " ")) ;
/*   while (stralter (str, "\n", "")) ; */
   return (1);
}


/*** STRALTER.  Alter 1st occurence of OLD in SSS to NEW. */

FUNCTION  stralter (sss, old, new)
   char  sss[], old[], new[];
{
   int   ssslen, oldlen, newlen, pos, delta;
   register int i;
   int   strlen(), strindex();

   ssslen = strlen (sss);   oldlen = strlen (old);   newlen = strlen (new);
   if (ssslen==0 || oldlen==0)         return (0);

   if ((pos = strindex (sss, old)) < 0)  return (0);

   delta = newlen - oldlen;
   if (delta > 0) {
      for (i=ssslen;  i>= pos+oldlen;  --i)  sss[i+delta] = sss[i]; }
   if (delta < 0) {
      for (i=pos+oldlen;  i<=ssslen;   ++i)  sss[i+delta] = sss[i]; }

   for (i=0;  i<newlen;  ++i)  sss[i+pos] = new[i];

   return (1);
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

FUNCTION   char *strtoken (word, pos, line)
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


/*** STRINDEX.  Return starting position in string S of string T. */

FUNCTION  strindex (s, t)
   char  s[], t[];
{
   register int  i, j, k;

   for (i=0;  s[i] != '\0';  i++) {
      for (j=i, k=0;  t[k] != '\0' && s[j]==t[k];  j++, k++) ;
      if (t[k] == '\0')  return (i);
   }
   return (-1);
}
