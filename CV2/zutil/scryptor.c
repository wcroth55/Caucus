
/*** SCRYPTOR.   Simple encryption/decryption program.
/
/    Invoked as:
/       scryptor  infile   outfile
/
/    Arguments:
/       infile     name of input file
/       outfile    name of output file
/
/    Purpose:
/       Scryptor reads INFILE, and either encrypts it or decrypts and
/       places the result in OUTFILE.
/
/    How it works:
/       Scryptor prompts the user for a key.  It then examines INFILE;
/       if it contains a magic code, it is an encrypted file and is
/       decrypted; otherwise it is a plain file and is encrypted.
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
/    Home:  scryptor.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/17/92 16:17 New program. */

#include <stdio.h>

#define  NOT         !
#define  FUNCTION

FUNCTION  main (argc, argv)
   int    argc;
   char  *argv[];
{
   FILE  *inp, *out, *fopen();
   char   magic[22], key[100];
   char  *s;
   int    decrypt, value;

   /*** Check args, open input & output files. */
   if (argc < 3) {
      printf ("Usage: scryptor infile outfile\n");
      exit   (1);
   }

   if ( (inp = fopen (argv[1], "r")) == NULL) {
      printf ("Can't open input file '%s'\n", argv[1]);
      exit   (1);
   }

   if ( (out = fopen (argv[2], "w")) == NULL) {
      printf ("Can't open output file '%s'\n", argv[2]);
      exit   (1);
   }

   printf ("Key? ");
   gets (key);

   /*** Check for magic number, and set DECRYPT true or false. */
   magic[0] = getc(inp);
   magic[1] = getc(inp);
   magic[2] = getc(inp);
   decrypt = (magic[0]==1  &&  magic[1]==2  &&  magic[2]==3);
   if (NOT decrypt) {
      fclose (inp);
      inp = fopen (argv[1], "r");
      putc (1, out);
      putc (2, out);
      putc (3, out);
   }

   s = key;
   while ( (value = getc(inp)) != EOF) {
      if (decrypt) value = (value - *s + 0x100) % 0x100;
      else         value = (value + *s)         % 0x100;
      putc (value, out);
      ++s;
      if (*s == '\0')  s = key;
   }

   fclose (inp);
   fclose (out);

   exit   (0);
}
