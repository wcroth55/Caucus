
/*** FREE_MASTER.   Deallocate a "master" structure made by make_master().
/
/    free_master (master);
/
/    Parameters:
/       struct master_template *master;     (ptr to master struct)
/
/    Purpose:
/       Deallocate the heap memory used by MASTER.
/
/    How it works:
/
/    Returns: 1 
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:  clear_master(), make_master().
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  resp/freemast.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/14/91 11:08 New function. */
/*: CR 11/18/91 17:46 Cast sysfree arg. */
/*: JX  6/16/92 16:41 Free organizerid. */
/*: JV 11/17/92 17:32 Change orighost from Chix to short. */
/*: CR 12/08/94 16:49 Remove clear_master() call. */
 
#include <stdio.h>
#include "caucus.h"

FUNCTION  free_master (m)
   struct master_template *m;
{
   ENTRY ("free_master", "");

   sysfree ( (char *) m->responses);
   sysfree ( (char *) m->orighost);
   sysfree ( (char *) m->origitem);
   if (m->organizerid != nullchix) chxfree (m->organizerid);

   RETURN  (1);
}
