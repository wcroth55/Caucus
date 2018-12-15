
/*** JIXREAD.   Read a line of bytes from the terminal, store in a Jix.
/
/    success = jixread (a, rep);
/
/    Parameters:
/       int   success;
/       Chix  a;       (read bytes, convert to jix, store in A)
/       int   rep;     (japanese representation used by terminal)
/
/    Purpose:
/       Read a line of bytes from the terminal, and store them in a
/       jix A.  REP is the code number for the japanese character
/       representation used by the terminal.
/
/    How it works:
/       Jixread() assumes that the caller has turned off terminal echo
/       and "cooking" (see systurn()).  Jixread() handles echoing of
/       characters typed by the user, as well as interpretation of
/       special and control characters.  (Jixread now calls systurn()
/       to find out if it *should* echo characters!)
/
/    Returns:
/       0 if successful.
/      -2 if the local end-of-file key was pressed at the start of a line
/      -3 cancel/interrupt
/      -4 on hang-up, or catastrophic i/o error
/      -5 error: A not chix, or bad value for REP
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
/    Home:  chx/jixread.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/24/91 14:45 New function. */
/*: CR  6/18/91 13:14 Add sjis handling code. */
/*: CX  6/02/92 14:49 Make return consistent with sysrdline(). */
/*: CX  6/04/92 14:01 Test for new jix... intermediate state. */
/*: CX  6/17/92 17:49 *DO* echo intermediate state byte. */
/*: CR 10/09/92 13:05 Call systurn() to find out if we should echo input. */

#include <stdio.h>
#include "caucus.h"
#include "chixdef.h"
#include "jix.h"
#include "diction.h"

FUNCTION  jixread (a, rep)
   Chix   a;
   int    rep;
{
   struct termstate_t termstate;
   int4   key;
   int    code, conv, init;
   char   str[2], temp[80], outseq[20];
   char  *output;

   if (NOT chxcheck (a, "jixread"))            return (-5);

   if (rep != LANG_EUC  &&  rep != LANG_SJIS)  return (-5);

   systurn (&termstate, GET_TERM, 0);

   chxclear (a);
   str[1] = '\0';
   for (init=0;   (1);   init=1) {
      code = sysget1key (&key);

      /*** Return immediately on special conditions: CANCEL, hangup,
      /    or EOF at start of line. */
      if (code == -3  ||  code == -4  ||  (code == -2 && init==0))
         return (code);

      /*** Newline, Return, or EOF (after some chars) means end of line. */
      if (code == -2  ||  key == 015  ||  key == 012) {
         if (termstate.c_echo >= ON)  sysputstr ("\r\n");
         break;
      }

      /*** Left-arrow and DEL are equivalent to backspace. */
      if (key == 70102L  ||  key == 0177)  key = 0x08L;

      if (key >= 70000) { sysputstr ("\007");   continue; }

      /*** Convert this one character, according to terminal type REP. */
      str[0] = (char) (key & 0xFF);
      if (rep == LANG_EUC)   conv = jixofeuc  (a, str, init, outseq);
      if (rep == LANG_SJIS)  conv = jixofsjis (a, str, init, outseq);

      /*** If any special commands or terminal mode changes need to be made,
      /    they were put in OUTSEQ by the jixof...() function.  */
      if (conv >= 0  &&  termstate.c_echo >= ON)  sysputstr (outseq);

      output = NULL;
      switch (conv) {
         case (0):                                     /* Regular char */
         case (1):  output = str;           break;  /* Intermediate state */
         case (2):  output = "\007";        break;  /* "Bad" char */
         case (3):  output = "\b \b";       break;  /* width-1 backspace */
         case (4):  output = "\b\b  \b\b";  break;  /* width-2 backspace */
         case (5):                          break;  /* Ignore char */
         default:
            sprintf   (temp, "(error %d)", conv);      /* Error */
            output = temp;
            break;
      }
      if (output != NULL  &&  termstate.c_echo >= ON)  sysputstr (output);
   }

   return (0);
}
