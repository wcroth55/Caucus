
/*** SWEBD_SCRIPTS.   Run 'swebd startup scripts'.
/
/    swebd_scripts (conf);
/   
/    Parameters:
/       SWconf;    (configuration struct)
/
/    Purpose:
/       swebd.conf allows specification of several scripts to be
/       run at swebd start-up time.  Run those scripts now.
/
/    How it works:
/
/    Known bugs:
/
/    Home:  sweb/swebdscr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/17/98 18:03 New function. */

#include <stdio.h>
#include <sys/types.h>
#include "chixuse.h"
#include "sweb.h"

FUNCTION  swebd_scripts (SWconf conf)
{
   int    sc;
   Chix   command;

   command = chxalloc (L(200), THIN, "swebd_scr");

   for (sc=0;   sc < MAX_SCRIPTS;   ++sc) {
      if (conf->scripts[sc] == NULL)  return;
      chxcpy (command, CQ(conf->scripts[sc]));
      func_shell (nullchix, 0, 0, 1, command, conf);
   }

   chxfree (command);
   return;
}
