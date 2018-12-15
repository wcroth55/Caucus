
/*** SYS SET.   Set values of system-dependent user parameters.
/
/    SYSSET acts identically to SET (in fact it is called from
/    SET), but works with the system-dependent user parameters only. 
/
/    If SYSSET is called with parameters, it displays the current
/    settings of all the system-dependent parameters.
/
/    If the parameter is "?", the help text for the system-dependent
/    parameters is displayed.  Otherwise, SET will set the value
/    of the selected parameter. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern Userreg            thisuser;
extern struct termstate_t term_in_caucus;
extern union  null_t      null;
extern Chix   sitevar[];

FUNCTION  sysset (out, command)
   int    out;
   Chix   command;
{
   ENTRY  ("sysset", "");
   RETURN (-1);
}
