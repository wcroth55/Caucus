
/*** UNIT_PUT.  Display STR on user's terminal, with conversion & wrapping.
/
/    unit_put (str, cancel);
/
/    Parameters:
/       Chix  str;      (text to be displayed)
/       int   cancel;   (does <CANCEL> interrupt display?)
/
/    Purpose:
/       Display text of chix STR on user's terminal, converting multi-byte
/       characters to the proper form for this terminal, word-wrapping
/       appropriately, and pausing as needed after each screenful.
/
/    How it works:
/       Unit_put() does the work of preparing the output for display,
/       primarily figuring out how to wrap over-int4 output.
/
/       The real output to the terminal is done by unit_term().
/       Pausing after each screenful is handled by unit_newline().
/
/       Unit_put() calls unit_control() to find out what kind of
/       character set is used to write the output.
/       
/    Returns:
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  unit_write() only!
/
/    Operating system dependencies: none
/
/    Known bugs: 
/
/    Home:  unit/unitput.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/13/92 11:33 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"

#define  SPACE   0    /* State values for output state table. */
#define  ASCII   1
#define  KANJI   2
#define  NEWLN   3

/*** Output character positions A through B of STR. */
#define OUTPUT(a,b)  chxclear  (temp);   CATSUB(a,b);   unit_term(temp)
#define CATSUB(a,b)  chxcatsub (temp, str, a, b-a+1)

extern struct screen_template screen, used;

FUNCTION  unit_put (str, cancel)
   Chix   str;
   int    cancel;
{
   int    state, input;
   int    success;   /* Used for debugging only, Lint will complain. */
   int4   pos, start, last, word;
   int4   value;
   Chix   temp;

   ENTRY ("unit_put", "");

   if (str == nullchix)  RETURN (1);

   /*** Unit_put() uses a 3 x 4 state table to handle all the possible
   /    transitions that affect the word-wrapping of output.  The states
   /    are:
   /       SPACE      in blank space, or at the very start of a line
   /       ASCII      in the middle of an ascii "word"
   /       KANJI      in the middle of kanji (each kanji acts as a word)
   /
   /    Each character being considered for output has a type (INPUT)
   /    that also has one of those three values, or is an explicit NEWLN.
   /
   /    The following variables are used to record transition information:
   /       POS        position of current character to be considered.
   /       START      beginning of block of text that can be output together.
   /       WORD       beginning of most recent "word".
   /       LAST       last non-blank character encountered. */

   /*** Consider each character in STR, and move through the state table. */
   temp  = chxalloc (L(100), THIN, "unitput temp");
   last  = -1;
   state = SPACE;
   for (word=pos=start=0;   value = chxvalue (str, L(pos));   ++pos) {

      /*** Figure out the width and type of the character at POS. */
      if      (value == ' ' )   input = SPACE;
      else if (value == '\n')   input = NEWLN;
      else if (value >  127 )   input = KANJI;
      else                      input = ASCII;
 
      /*** At a newline, display everything from START to LAST, with a
      /    newline, and move START to right after the newline. */
      if (input == NEWLN) {
         OUTPUT (start, last);
         unit_newline();
         if (cancel  &&  sysbrktest())  SUCCEED;
         start = pos+1;
         state = SPACE;
         continue;
      }

      /*** If a space pushes us past the wrap limit, display everything
      /    from START to LAST (with a newline), skip to the next non-blank
      /    character, and "START" again from there. */
      if (input == SPACE) {
         if (++used.width > screen.width) {
            OUTPUT (start, last);
            unit_newline();
            if (cancel  &&  sysbrktest())  SUCCEED;

            for (++pos;   (value = chxvalue (str, L(pos))) == ' ';   ++pos) ;
            if (value == '\n')  ++pos;
            start = pos;
            --pos;
         }
         state = SPACE;
         continue;
      }

      /*** A regular ascii character either adds to a word or starts a
      /    a new word.  If we go beyond the wrap limit, attempt to wrap
      /    an entire word. */
      if (input == ASCII) {
         if (++used.width > screen.width) {
            /*** If this is a very int4 word, just chop it at POS. */
            if (word <= start)  word = pos;

            /*** Display up to the beginning of the most recent word,
            /    and then wrap to the next line. */
            OUTPUT (start, word-1);
            unit_newline();
            if (cancel  &&  sysbrktest())  SUCCEED;
            pos   = word-1;
            start = word;
            state = SPACE;
            continue;
         }
         if (state != ASCII)  word = pos;
         last  = pos;
         state = ASCII;
         continue;
      }

      /*** Kanji characters are words unto themselves, as far as word-
      /    wrapping goes. */
      if (input == KANJI) {
         used.width += jixwidth (value);
         if (used.width > screen.width) {
            OUTPUT (start, last);
            unit_newline();
            if (cancel  &&  sysbrktest())  SUCCEED;
            start = pos;
            state = SPACE;
            --pos;
            continue;
         }
         last  = pos;
         state = KANJI;
         continue;
      }
   }

   /*** When we run out of chars, output what remains. */
   OUTPUT  (start, pos-1);

done:
   chxfree (temp);
   RETURN  (1);
}
