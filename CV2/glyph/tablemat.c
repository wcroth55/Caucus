/*** TABLEMATCH.  WORD may be an abbreviation for a word in TABLE.  Find it.
/
/    n = tablematch (table, word);
/
/    Parameters:
/       int   n;       (word is N'th entry in table)
/       Chix  table;   (list of words)
/       Chix  word;    (word being searched for)
/
/    Purpose:
/       Tablematch() searches through the words in TABLE to see if WORD
/       is an abbreviation for any of them.
/
/    How it works:
/       Definition: WORD is an abbreviation of a word Q in table if WORD
/       is an initial substring of Q.
/
/       A table is a single string, containing a list of tokens separated
/       by blanks.  Each token consists of one or more words, separated by
/       slashes.  For example, here's a simple table with 3 entries:
/
/          "yes  no  help/?"
/
/       Entry 0 is "yes", 1 is "no".  Entry 2 is either "help" or "?". 
/    
/    Returns:
/      -2  if WORD is an abbreviation for 2 or more entries in TABLE
/      -1  if WORD is an abbreviation for NO entries in TABLE
/       n  if WORD is an abbreviation for entry N (0, 1, 2...) in TABLE.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  application
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  glyph/tablemat.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:53 Source code master starting point */
/*: AM  2/07/89 10:33 Jcaucus changes*/
/*: CR  1/27/90 16:12 Fix *&^%! ENTRY/RETURN to be consistent! */
/*: CX  5/07/92 18:33 Remove dependence on extern null. */
/*: CX  5/19/92 13:41 Chixifed version. */
/*: JX  6/11/92 12:34 Initialize wp. */
/*: CR 12/11/92 13:53 Long chvalue arg. */

#include <stdio.h>
#include "caucus.h"
 
#define  STARTWORD   1
#define  CHECKWORD   2
#define  NEXTPART    3
#define  NEXTWORD    4

FUNCTION  tablematch (table, word) 
   Chix   table, word;
{ 
   int4   s,  w;      /* table, word: character values. */
   int4   sp, wp;     /* table, word: "pointers". */
   int4   stop;
   int    state, which, res;

   ENTRY ("tablematch", "");

   if (table==nullchix  ||  word==nullchix  ||
       EMPTYCHX(table)  ||  EMPTYCHX(word))  RETURN (-1);

   state = STARTWORD;
   which =  0;
   res   = -1;
   stop  = chxlen(table) + 1;
   wp    = 0;

   /*** TABLEMAT is implemented as a finite-state machine.  As we march
   /    S character by character through TABLE, we are either:
   /      STARTWORD   looking for start of next token in TABLE
   /      CHECKWORD   checking WORD against word in current token
   /      NEXTPART    looking for next word in current token
   /      NEXTWORD    skipping to end of token.
   /
   /    Processing terminates when S goes BEYOND the null character at
   /    the end of TABLE.  Note that if we get a match (set RES) twice,
   /    then WORD is ambiguous and we return -2. */

   for (sp=0;   sp != stop;   ) {
      s = chxvalue (table, sp);
      w = chxvalue (word,  wp);
      switch (state) {

         case (STARTWORD):
            if (NOT chxblank(s))   { wp = 0;            state = CHECKWORD; }
            else                                ++sp;
            break;
   
         case (CHECKWORD):
            if      (w== 0  &&  res>=0)                 RETURN(-2)
            else if (w== 0)        { res = which;       state = NEXTWORD; }
            else if (w==s)         { ++wp;      ++sp;                    }
            else                                        state = NEXTPART;
            break;
   
         case (NEXTPART):
            if      (s=='/')       { wp = 0;     ++sp;  state = CHECKWORD; }
            else if (chxblank(s))  { ++which;           state = STARTWORD; }
            else                                 ++sp;
            break;
   
         case (NEXTWORD):
            if      (chxblank(s))  { ++which;           state = STARTWORD; }
            else                                 ++sp;
            break;
      }
   }

   RETURN (res);
}
