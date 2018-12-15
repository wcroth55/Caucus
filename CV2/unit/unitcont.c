
/*** UNIT_CONTROL.   Control the behaviour of unit I/O.
/
/    value = unit_control (n, code);
/
/    Parameters:
/       int   value;   (old code value for unit N)
/       int   n;       (unit number to be affected)
/       int   code;    (control code)
/
/    Purpose:
/       Control or interrogate the behaviour of I/O on unit N.
/
/    How it works:
/       N <= XITT controls the behaviour of terminal output.
/       All other values of N control disk I/O.
/
/       Currently, unit_control() has two aspects: language/character
/       representation, and terminal output substitution.
/
/    Language representation:
/       lang = unit_control (N, UNIT_GET);
/          Returns the current character representation in use for N.  
/
/       old  = unit_control (N, UNIT_SET + lang);
/          Sets the current character representation for N to LANG.
/          Returns the previously used representation.
/
/       LANG must be one of the language elements from diction.h, e.g.
/       LANG_NORM, LANG_ISO, LANG_EUC, LANG_SJIS, etc.
/
/    Terminal output substitution:
/       unit_control (XITT, UNIT_SETSUB + u);
/          Redirects all terminal output to unit U, which must already
/          be locked and open.  If U is 0, reverts output to terminal.
/
/       u = unit_control (n, UNIT_GETSUB);
/          Returns the actual unit number used for unit N.
/
/    Returns: see above.
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  unit/unitcont.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/12/92 16:39 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"
#include "diction.h"

extern struct termstate_t term_in_caucus;

FUNCTION  unit_control (n, code)
   int  n, code;
{
   static int tty_value = LANG_NORM;
   static int dsk_value = LANG_NORM;
   static int tty_sub   = -1;
   int        old, lowbits, result, hibits;

   ENTRY ("unit_control", "");

   lowbits = code &   UNIT_MASK;
   hibits  = code & (~UNIT_MASK);

   if (hibits==UNIT_SET  ||  hibits==UNIT_GET) {
      if (n <= XITT) {
         old = tty_value;
         if (code & UNIT_SET)  tty_value = lowbits;
   
         /*** Switching from NORM to anything else, turn off echo/cooking. */
         if (old == LANG_NORM  &&  tty_value != LANG_NORM) {
            systurn (&term_in_caucus, OS_ECHO,    OFF);
            systurn (&term_in_caucus, COOK_INPUT, OFF);
            systurn (&term_in_caucus, EIGHT_BIT,  ON );
         }
   
         /*** Switch from japanese (any form) to NORM, turn on echo/cooking. */
         else if (old != LANG_NORM  &&  tty_value == LANG_NORM) {
            systurn (&term_in_caucus, OS_ECHO,    ON);
            systurn (&term_in_caucus, COOK_INPUT, ON);
            systurn (&term_in_caucus, EIGHT_BIT,  OFF);
         }
      }
   
      else {
         old = dsk_value;
         if (code & UNIT_SET)  dsk_value = lowbits;
      }

      result = old;
   }

   if (hibits == UNIT_SETSUB  &&  n <= XITT) {
      if (lowbits > 0)   result = tty_sub = lowbits;
      else               result = tty_sub = -1;
   }

   if (hibits == UNIT_GETSUB) {
      if (n <= XITT  &&  tty_sub >= 0)  result = tty_sub;
      else                              result = n;
   }
      
   RETURN (result);
}
