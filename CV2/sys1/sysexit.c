
/*** SYSEXIT.  System-dependent exit to operating system.
/
/    SYSEXIT is called with one argument, the error STATUS.
/    Zero means "normal exit"; all other values have some meaning.
/
/   (If STATUS is -1, it has the special meaning "initialize".
/    This special call is made from MAIN when the program begins,
/    and initializes some special condition and then returns.)
/
/    SYSEXIT must undo any special conditions set up by Caucus when
/    it began exiting, such as Unix umasks, or VM/CMS IUCV connection
/    to the server machine. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR 10/10/05 Remove sysmdstr, systurn calls for C5. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include <sys/stat.h>
#include <sys/types.h>

extern struct flag_template flag;
extern struct termstate_t   term_original;
extern Chix   servername;
extern union  null_t null;

FUNCTION  sysexit (status)
   int    status;
{
   int    junk;

#if WNT40
   if (status != -1)  exit (status);
#endif

#if UNIX | NUT40
   static int    umaskval = 99;

   if (status == -1) {
      umaskval = umask((mode_t) 077);
   }
   else {
      if (umaskval != 99) umask ((mode_t) umaskval);
      exit     (status);
   }
#endif

}
