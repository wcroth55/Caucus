
/*** JIXOFEUC.  Convert stream of 8-bit bytes in EUC to Jix.
/
/    code = jixofeuc (a, b, init, outseq);
/
/    Parameters:
/       int   code;
/       Chix  a;
/       char *b;
/       int   init;
/       char *outseq;
/
/    Purpose:
/       Convert a string of japanese characters, represented as a
/       array of 8-bit bytes in B in the EUC format, into jix, and
/       add them onto the end of the jix (chix) string in A.  A is
/       forced to type WIDE.
/
/       The converted bytes from B are added onto the end of A.  
/       The first call for a new jix A should always have INIT==0.
/
/    How it works:
/       Jixofeuc() converts any number of incoming EUC bytes into Jix, and
/       adds them on to the end of A.  Jixofeuc can handle a variety of
/       special cases, including:
/
/         1. B contains a single byte of a multi-byte EUC character
/            (Jixofeuc keeps an internal state machine to keep track of
/             which byte of a multi-byte sequence comes next)
/
/         2. B contains "bad" (control or) other characters.  These are
/            filtered out and do not become part of the jix string A.
/
/         3. B contains backspaces (^H).  Each backspace erases the
/            previous entire *character*.
/
/       OUTSEQ is provided for similarity with other jixof...() functions.
/       It is always cleared, i.e. set to the empty string.
/       
/    Returns: value depends on last byte in B:
/       0   conversion okay
/       1   conversion okay, but last byte left us in intermediate state
/       2   "bad" character, should be ignored
/       3   backspace, erased single-width character
/       4   backspace, erased double-width character
/       5   ignore, do nothing (such as backspace at start of line!)
/      -1   Error!  A not chix.
/      -2   Error!  New jix A without INIT==0.
/      -3   Error!  Jix A could not be expanded large enough.
/
/    Error Conditions:  See return values < 0.
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  chx/jixofeuc.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/24/91 14:45 New function. */
/*: CR  6/18/91 17:36 Fix error: EUC starts at 0xA1, not 0x81. */
/*: CR  1/03/92 13:39 Error -3 if A cannot be resized. */
/*: CX  6/04/92 13:54 Add "intermediate state" return code. */
/*: CR 12/11/92 13:23 Chxresize last arg is int4 *. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  jixofeuc (a, b, init, outseq)
   Chix   a;
   char  *b;
   int    init;
   char  *outseq;
{
   static Chix last_jix = ((Chix) NULL);
   static int4 pos;
   static int  state;
   int    i, code, highbyte, byte;

   ENTRY ("jixofeuc", "");

   /*** Input validation. */
   if (b      == NULL)  b = "";
   if (outseq != NULL)  outseq[0] = '\0';
   if (NOT chxcheck (a, "jixofeuc"))  RETURN (-1);
   
   /*** If we are working with a new jix A, initialize.  This means resize
   /    to WIDE, clear the STATE variable, and point POS at the start of A. */
   if (init == 0) {
      last_jix = a;

      /*** Original code assumed A was to be cleared; this actually
      /    causes a mess.
         chxclear (a); 
         pos = 0;
      /  */

      if (a->type != WIDE) {
         if (NOT chxresize (a, WIDE, a->maxlen))  RETURN (-3);
         a->type = WIDE;
      }
      state = 0;
      pos   = chxlen(a);    /* Instead of pos = 0; */
   }

   /*** Check that jix A was initialized with INIT==0. */
   if (last_jix != a)  RETURN (-2);

   /*** Examine each character in B, and convert appropriately.  
   /    STATE==0 means ready for new character.  STATE==1 means ready
   /    for 2nd byte of current 2-byte character.  CODE is our eventual
   /    return value. */
   for (code=0;   *b;   ++b) {
      byte = *b & 0xFF;
      code =  0;

      /*** Resize A if we get too close to its maximum length. */
      if (pos > a->maxlen - 5)  
         if (NOT chxresize (a, WIDE, pos + 100))  RETURN (-3);

      /*** POS is the "current" character position.  I is the equivalent
      /    byte index.  Add a zero terminator character after POS. */
      i = 2 * pos;
      a->p [i+2] = '\0';
      a->p [i+3] = '\0';

      /*** Ignore backspaces at start of A. */
      if (byte == 0x08  &&  pos == 0)  { code = 5;   continue; }

      /*** Handle backspace over single or double width character. */
      if (byte == 0x08) {
         --pos;
         i = 2 * pos;
         highbyte = (a->p[i] & 0xFF);

         /*** High bit of high byte on means double width, except for SS2
         /    sequence for sw-kana. */
         code = ( (highbyte & 0x80)  &&  (highbyte!=0x8E) ? 4 : 3);
         a->p [i  ] = '\0';
         a->p [i+1] = '\0';
         continue;
      }

      /*** Illegal bytes. */
/*    if (byte < 040)  { code = 2;   continue; } */
/*    if (byte != 0x0A  &&  byte < 040)  { code = 2;   continue; } */

      /*** Process and convert character bytes. */
      if (state == 0) {
         if ( (byte >= 0x80  &&  byte <= 0x85)  ||
              (byte >= 0x90  &&  byte <= 0xA0)  ||
              (byte == 0xFF))  { code = 2;   continue; }

         if (byte &  0x80) {
            a->p [i  ] =  *b;
            a->p [i+1] = '\0';
            state = code = 1;
         }
         else {
            a->p [i  ] = '\0';
            a->p [i+1] =  *b;
            ++pos;
         }
      }
      else {
         a->p [i+1] = *b;
         state = 0;
         ++pos;
      }
   }

   a->actlen = -1L;
   RETURN (code);
}
