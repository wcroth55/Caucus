/*** SYSSCRIPT.  Execute an operating system command script.
/
/    ok = sysscript (line);
/
/    Parameters:
/       int    ok;          (Success?)
/       char  *line;        (text of ?)
/
/    Purpose:
/       Execute an operating system script, with arguments.
/
/    How it works:
/       The name of the script file, followed by its arguments, is
/       in LINE.  Sysscript() prepends any operating-system dependent
/       stuff needed to say "execute this script", and passes the whole
/       thing off to syscmd() to execute.
/
/    Returns: 1 on success, 0 on failure.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by: application
/
/    Operating system dependencies: extreme
/
/    Known bugs:
/
/    Home:  sys/sysscrip.c
/
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/01/91 14:24 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "sys.h"
#include "systype.h"

FUNCTION  sysscript (line)
   char  *line;
{

#if NUT40 | WNT40
   return (system (line) != -1);
#endif


#if UNIX
   char    temp[300];
   ENTRY   ("sysscript", "");
   strconc (temp, "exec ", line);
   RETURN  (syscmd (temp, SYS_WAIT) > 0);
#endif

}
