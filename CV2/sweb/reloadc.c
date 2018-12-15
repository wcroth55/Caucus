
/*** RELOAD_CHECK.   Is this page request a RELOAD?
/
/    reload = reload_check (cmlname, query);
/   
/    Parameters:
/       int   reload;     (return 1 or 0)
/       char *cmlname;    (name of CML page)
/       char *query;      (full query string)
/
/    Purpose:
/       Determine if the current page request is actually a RELOAD
/       (press of browser RELOAD button) of a recent page.
/
/    How it works:
/
/    Returns: 1 if yes, 0 if no
/
/    Known bugs:
/
/    Home:  sweb/reloadc.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/09/95 17:33 New function. */
/*: CR  3/25/95 13:13 Convert to chix. */
/*: CR  8/14/98 12:56 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "sweb.h"

#define  CMAX  256
#define  RLMAX  10

static char recent[RLMAX][CMAX];
static int  ptr = -1;

FUNCTION  reload_check (char *cmlname, char *query)
{
   int    num;
   char   full[CMAX];

   /*** Initialization. */
   if (ptr < 0) {
      for (num=0;   num<RLMAX;   ++num)  recent[num][0] = '\0';
      ptr = 0;
   }

   /*** Build the full URL. */
   strcpy (full, cmlname);
   strcat (full, query);

   /*** Is it in the table of recent URLs? */
   for (num=0;   num<RLMAX;   ++num)
      if (streq (full, recent[num]))   return (1);

   /*** No, but add it on to the end of the (circular) table. */
   strcpy (recent[ptr], full);
   ptr = (ptr + 1) % RLMAX;

   return (0);
}

