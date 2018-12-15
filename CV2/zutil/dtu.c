/*** DTU.  Program to convert DOS text format to Unix text format.
/         
/    Invoked as:
/       dtu <dosfile >unixfile
/
/    Arguments:
/       dosfile    "input" file in DOS (or other) text format
/       unixfile   "output" file, to be produced in Unix text format
/
/    Purpose:
/       Determine if input file is in DOS (or other non-Unix) text
/       format.  If it is, convert the data to Unix text format and
/       write to the output file.
/
/    How it works:
/       Use the mappings shown below to do text conversion:
/         ^h   --> ^h    (BS)
/         ^i   --> ^i    (TAB)
/         ^j^m --> \n    (LF_CR)
/         ^m^j --> \n    (CR_LF)
/         ^j   --> \n    (LF)
/         ^m   --> \n    (CR)
/         ^l   --> ^l    (FF)
/         \0's at end of input file are ignored and thrown away
/              (may be caused by binary xmodem transfers)
/
/       If any other non-printable ascii characters appear, this is
/       not a text file.  Exit with error status.
/
/    Exit status: 0 on success
/                 1 on binary file
/                 2 on empty file
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
/       DTU will allow the very last character of a file to be non-ascii,
/       and still consider the file to be ascii.  This may be a good thing.
/
/    Home:  zutil/dtu.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JZ 10/07/93 16:08 Create this. */
/*: CP 11/19/93 16:14 Rewrite header comments; change c, d to int. */
/*: CK 12/02/93 14:24 Rearrange logic, fix last char=^M bug. */

#include <stdio.h>

#define  BS     8
#define  TAB    9
#define  LF    10
#define  CR    13
#define  FF    12
#define  CTLZ  26

main (argc, argv)
   int argc; char *argv[];
{
   int  c, d;
   int  binary=0, pad=0;

   /* Read 1 byte from STDIN. */
   if ((c = getchar()) == EOF) return(2);

   while ((d = getchar()) != EOF) {

      /* Write char's that aren't CR. */
      if (ascii (c))  {
         /* Don't allow ASCII after NULLs */
         if (pad) { binary=1; break; }
         putchar (c);
      }

      /* LF & CR together -> \n.   LF alone -> \n. */
      else if (c==LF) {
         if (d==CR) { if ( (d = getchar()) == EOF) break; }
         putchar ('\n');
      }

      /* CR & LF together -> \n.   CR alone -> \n. */
      else if (c==CR) {
         if (d==LF) { if ((d = getchar()) == EOF) break; }
         putchar ('\n');         
      }

      /* non-ASCII */
      else if (c == 0)   pad=1;      /* Skip terminating (padding) nulls */

      /* It's *really* not ASCII */
      else { binary=1; break; }

      /* Go back and check next char. */
      c = d;
   }

   /* Write out last character if not ^Z or padding. */
   if (c == CR)                c = LF;
   if (c != CTLZ  &&  c != 0)  putchar (c);

   return (binary ? 1 : 0);
}

ascii (c)
char c;
{
   if (c==BS || c==TAB)             return (1);
   if (c >= ' '  &&  c <= '~')      return (1);
   return (0);
}
