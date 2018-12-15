/*** TABLEPART.  A word in TABLE may be an initial substring of WORD.  Find it.
/
/    n = tablepart (table, word);
/
/    Parameters:
/       int   n;       (N'th entry in table is substr of WORD)
/       Chix  table;   (list of words)
/       Chix  word;    (word being matched against)
/
/    Purpose:
/       Tablepart() searches through the words in TABLE to see if any
/       of them are an initial substring of WORD.
/
/    How it works:
/    
/    Returns:
/       n  if entry N is a substring of WORD.
/      -1  if no entries match WORD.
/
/    Error Conditions:
/  
/    Called by:  application
/
/    Known bugs:      none
/
/    Home:  glyph/tablepart.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/11/95 17:46 New function. */

#include <stdio.h>
#include "caucus.h"
 
#define  BLANK  0
#define  TEST   1
#define  SKIP   2

FUNCTION  tablepart (table, word) 
   Chix   table, word;
{ 
   int4  pos, start, val;
   int   which;            /* word number we're on in TABLE. */
   int   state;

   if (table==nullchix  ||  word==nullchix  
                        ||  chxvalue (table, L(0)) == 0
                        ||  chxvalue (word,  L(0)) == 0)  return (-1);

   state = BLANK;
   which = -1;
   for (pos=0;   (val = chxvalue (table, pos));   ++pos) {

      if      (state == BLANK) {
         if (chxblank(val))  continue;
         ++which;
         state = TEST;
         start = pos--;
      }

      else if (state == TEST) {
         if (chxblank(val))                      return (which);
         if (val != chxvalue (word, pos-start))  state = SKIP;
      }

      else if (state == SKIP) {
         if (chxblank(val))  state = BLANK;
      }
   }

   if (state == TEST)  return (which);

   return (-1);
}
