
/*** UNIT_NEWLINE.  Display newline on user's terminal, pausing appropriately.
/
/    unit_newline();
/
/    Parameters:
/
/    Purpose:
/       Display a newline on the user's terminal.  Pause after each
/       screenful, as appropriate.
/
/    How it works:
/       If screen has a definite size, pause after that many lines
/       have been written, and prompt the user to press RETURN.
/       Overwrite the prompt with blanks so that it does not appear
/       in the text after the prompt has been answered.
/       
/    Returns:
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  unit_put() only!
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  unit/unitnewl.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/14/92 11:33 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR 10/12/05 Remove all terminal capabilities for C5. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"

extern struct screen_template screen, used;
extern struct termstate_t     term_in_caucus;
extern int                    debug;

FUNCTION  unit_newline()
{
   ENTRY ("unit_newline", "");

   /*** Actually display a newline.  After *that*, we'll figure out
   /    if we are at the end of a screenful, and what to do about it. */
   sysputnl();
   used.width = 0;

   RETURN(1);
}
