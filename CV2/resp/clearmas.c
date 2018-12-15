
/*** CLEAR_MASTER.  Clear out chix used in a master_template.
/
/    clear_master (m);
/
/    Parameters:
/       struct master_template *m;
/
/    Purpose:
/       The master_template contains basic information about all the
/       items in a conference, including # of items, # of responses
/       per item, item # on original host (distributed), and original
/       host name.
/
/       Some of this information is stored in a sparse array of chix.
/       This array should be cleared out before joining a new conference.
/
/    How it works:
/       Find all the original host pointers that are not null, and
/       free those chix.
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  join().
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  resp/clearmas.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/19/91 14:26 New function. */
/*: JV 11/17/92 17:30 Change orighost from Chix to short. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  clear_master (m)
   struct master_template *m;
{
   int    i;

   ENTRY ("clear_master", "");

   for (i=0;   i<=m->items;   ++i)
      m->orighost[i] = 0;

   RETURN (1);
}
