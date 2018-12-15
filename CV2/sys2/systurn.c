/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSTURN.  Control terminal input processing: echoing, "cooking", etc.
/
/    systurn (termstate, what, value);
/
/    Parameters:
/       struct termstate_t *termstate;    (terminal state block)
/       int                 what;         (aspect of terminal state)
/       int                 value;        (value to set state to)
/
/    Purpose:
/       Caucus reads input from the user's terminal via two functions:
/       sysrdline() and sysget1key().  Inside these two functions, several
/       kinds of input processing occur: echoing, 7/8 data bit selection, and
/       "cooking".  (Cooking refers to the processing of control and editing
/       characters.)
/
/       SYSTURN provides control over this input processing.  The arguments
/       to SYSTURN are TERMSTATE, WHAT and VALUE.  TERMSTATE is a control block
/       that represents a terminal "state".  WHAT and VALUE specify what
/       should be done to a terminal state:
/
/    How it works:
/
/       WHAT        VALUE  ACTION
/       ----------  -----  ----------------------------------------------
/       GET_TERM           Get terminal state and save in STATE.
/       SET_TERM           Set terminal state to contents of STATE.
/       CAUCUS_ECHO ON     Where appropriate, Caucus echos input typed by user.
/       CAUCUS_ECHO OFF    Caucus does not echo input typed by user.
/       OS_ECHO     ON     Where appropriate, operating sys echoes input.
/       OS_ECHO     OFF    OS does not echo input.
/       COOK_INPUT  ON     Input typed by user is pre-processed by O.S.
/       COOK_INPUT  OFF    Input typed by user enters Caucus "raw".
/       EIGHT_BIT   ON     Caucus reads full 8 bit data.
/       EIGHT_BIT   OFF    Caucus reads 7 low order bits of each byte.
/       LINE_EDIT   ON     Caucus/VV allows line-editing characters
/       LINE_EDIT   OFF    Caucus/VV uses ^H for backspace
/       IS_AFILE    ON     I/O is to pipe or file; don't ioctl it!
/       IS_AFILE    OFF    I/O is to terminal (default)
/
/       CAUCUS_ECHO, OS_ECHO, COOK_INPUT, EIGHT_BIT, & LINE_EDIT also perform
/       a "SET_TERM" in addition to changing their specific feature.  Any
/       combination of the WHAT's may be added together, however they will
/       all be set to the same VALUE.  Also, if GET_TERM is specified, any
/       other WHAT's are ignored.
/ 
/       Multiple calls to SYSTURN are stacked.  That is, after the sequence
/       of calls shown:
/          systurn (&state, CAUCUS_ECHO, OFF);
/          systurn (&state, CAUCUS_ECHO, OFF);
/          systurn (&state, CAUCUS_ECHO, ON);
/
/       Caucus input echoing is still off.  One more "CAUCUS_ECHO, ON" call
/       will turn Caucus echoing back on.  Both echoing and cooking are assumed
/       to be ON to begin with.  Calls to systurn() for CAUCUS_ECHO, OS_ECHO,
/       and COOK_INPUT may be made in *any* order, and will still be processed
/       properly.
/ 
/       Notes for specific system type codes:
/
/       CVM, PX: systurn() is a noop.
/
/       CA: sysexit() tells the operating system to turn off echoing and
/           cooking.  All echoing/cooking is done inside sysread()/sysgetc().
/
/       IM, PN, VV: all echo/cooking work is done in sysrdline()/sysgetc().
/
/    Returns:
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions: sysrdline()
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  sys/systurn.c
/ */

/*: CR  4/09/90 16:32 New function.  Heavy rewrite of systurn(), syscook(). */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "sys.h"

/*** Declarations of things needed for different op/sys to really turn
/    echo and cooking on and off. */
static char          first_call = 1;
static char          ok = 0;          /* False if first_call ioctl fails. */

#if SYSV | HPXA9 | SUN41 | LNX12
#include <stdio.h>
#include <termio.h>
static struct termio original, new;
#endif

#if BSD21 | FBS22
#include <stdio.h>
#include <termios.h>
#define  TCGETA  TIOCGETA
#define  TCSETA  TIOCSETA
static struct termios original, new;
#endif


FUNCTION  systurn (termstate, what, value)
   struct termstate_t *termstate;
   int    what, value;
{
   extern struct sysio_t sysio;

   ENTRY ("systurn", "");

   if (what & GET_TERM) { *termstate = sysio.state;   RETURN(1); }

   /*** Keep "stack" of calls as a count of number ON's and OFF's of each
   /    type in structure. */
   if (what & OS_ECHO)      termstate->o_echo   += (value==ON ? 1 : -1);
   if (what & CAUCUS_ECHO)  termstate->c_echo   += (value==ON ? 1 : -1);
   if (what & COOK_INPUT)   termstate->cooked   += (value==ON ? 1 : -1);
   if (what & EIGHT_BIT)    termstate->eightbit += (value==ON ? 1 : -1);
   if (what & LINE_EDIT)    termstate->lineedit += (value==ON ? 1 : -1);
   if (what & IS_AFILE)     termstate->is_afile += (value==ON ? 1 : -1);


   /*** Any of SET_TERM, ECHO_INPUT, COOK_INPUT, EIGHT_BIT, or LINE_EDIT
   /    also set sysio state to termstate. */
   if (sysio.state.is_afile)  RETURN(1);
   sysio.state = *termstate;
   if (sysio.state.is_afile)  RETURN(1);


/*------------------------System V, including SunOS!----------------------*/
#if UNIX
   if (first_call) {
      first_call = 0;
      ok = (ioctl (0, TCGETA, &original) >= 0);
      sysio.c_intr  = original.c_cc[0];
      sysio.c_erase = original.c_cc[2];
      sysio.c_eof   = original.c_cc[4];
   }

   new = original;
   if (sysio.state.o_echo <= 0)    new.c_lflag = new.c_lflag & (~ECHO);
   if (sysio.state.cooked <= 0) {
      new.c_lflag = new.c_lflag & (~ICANON);
      new.c_cc[4] = 1;             /* Minimum chars to recv */
      new.c_cc[5] = 0;             /* Minimum time to wait for them */
   }
   if (sysio.state.eightbit > 0)   new.c_iflag = new.c_iflag & (~ISTRIP);

#if HPXA9 | SUN41 | BSD21 | FBS22
   if (ok) ioctl (0, TCSETA,  &new);
#endif
#if SYSV  | LNX12
   if (ok) ioctl (0, TCSETAF, &new);
#endif

#endif

   RETURN (1);
}
