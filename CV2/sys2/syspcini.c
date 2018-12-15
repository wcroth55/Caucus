/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSPCINIT.   (Re)-Initialize PC communications port. 
/
/    If this is a PC in remote mode, SYSPCINIT initializes the PC 
/    communications port, and returns 1.  Otherwise, it returns 0.
/
/    SYSPCINIT is called once at the beginning of Caucus, and then
/    again (from "loginshell") whenever the carrier-detect signal
/    drops (i.e. sysgetch returns -1, or prompter returns -4).  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern struct flag_template flag;
extern union  null_t        null;

FUNCTION  syspcinit()
{
   ENTRY  ("syspcinit", "");
   RETURN (0);
}

