
/*** STRTABLE.  Find number of WORD in TABLE.
/
/    n = strtable (table, word);
/
/    Parameters:
/       int   n;       (word is N'th entry in table)
/       char *table;   (list of words)
/       char *word;    (word being searched for)
/
/    Purpose:
/       Strtable() searches through the words in TABLE to see if WORD
/       is identical to one of them.
/
/    How it works:
/    
/    Returns:
/       n <  0  if WORD cannot be found
/       n >= 0  if WORD is entry N (0, 1, 2...) in TABLE.
/
/    Error Conditions:
/  
/    Known bugs:      none
/
/    Home:  str/strtable.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/25/98 14:06 New function */

#include <stdio.h>
#include "caucus.h"

#define  BETWEEN     1
#define  CHECKWORD   2
#define  SKIPWORD    3

#define  ZERO(x)    (!(x))

FUNCTION  strtable (char *table, char *word) 
{ 
   char  *sp, *wp;
   int    state, which;

   ENTRY ("strtable", "");

   if (ZERO(table[0])  ||  ZERO(word[0]))  RETURN (-1);

   state = BETWEEN;
   which = -1;
   wp    = word;

   /*** STRTABLE is implemented as a finite-state machine.  As we march
   /    SP character by character through TABLE, we are either:
   /      BETWEEN     looking for start of next token in TABLE
   /      CHECKWORD   checking WORD against word in current token
   /      SKIPWORD    skipping to end of current token.
   /
   /    Processing terminates when SP goes BEYOND the null character at
   /    the end of TABLE.  */

   sp = table;
   while (1) {
      switch (state) {
         case (BETWEEN):
            if (ZERO(*sp))                             RETURN (-2);
            if (*sp == ' ')     ++sp;
            else         {   wp = word;    ++which;    state = CHECKWORD; }
            break;
   
         case (CHECKWORD):
            if (ZERO(*wp)  && (*sp==' ' || ZERO(*sp))) RETURN (which);
            if (ZERO(*sp))                             RETURN (-3);
            if (*sp == *wp)                          { ++sp;   ++wp; }
            else                                       state = SKIPWORD;
            break;
   
         case (SKIPWORD):
            if (ZERO(*sp))                             RETURN (-4);
            if (*sp == ' ')                            state = BETWEEN;
            ++sp;
            break;
      }
   }
   RETURN (-5);   /* Can't get here. */
}
