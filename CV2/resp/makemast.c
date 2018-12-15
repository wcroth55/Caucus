
/*** MAKE_MASTER.   Allocate an item/response "master" structure.
/
/    ok = make_master (master, maxitems);
/
/    Parameters:
/       int                     ok;         (success?)
/       struct master_template *master;     (ptr to unallocated master struct)
/       int                     maxitems;   (max number of items)
/
/    Purpose:
/       A "master" structure has members that must be allocated off the
/       heap before the structure can be used.  Make_master() allocates
/       and initializes these members to allow for MAXITEMS items.
/
/    How it works:
/
/    Returns: 1 on success
/             0 if any memory allocation failed.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:  clear_master(), free_master().
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  resp/makemast.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/14/91 11:08 New function. */
/*: WC 11/30/91 15:15 Exit if out of memory. */
/*: JX  6/16/92 16:40 Allocate organizerid. */
/*: JV 11/17/92 17:33 Change orighost from Chix to short. */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: CR  2/28/93 23:10 Initialize 'lastused' member. */
 
#include <stdio.h>
#include "caucus.h"
#include "sysexit.h"

FUNCTION  make_master (m, maxitems)
   struct master_template *m;
   int    maxitems;
{
   int   i;
   char *sysmem();

   ENTRY ("make_master", "");

   m->responses    = (short *) sysmem (L(sizeof(short)) * L(maxitems + 3), "mresp");
   m->origitem     = (short *) sysmem (L(sizeof(short)) * L(maxitems + 3), "morig");
   m->orighost     = (short *) sysmem (L(sizeof(short)) * L(maxitems + 3), "mhost");
   m->organizerid  = chxalloc (L(16), THIN, "master organizerid");
   m->lastused     = 0;

   if (m->responses == (short *) NULL  ||
       m->origitem  == (short *) NULL  ||
       m->orighost  == (short *) NULL)  /*  RETURN (0); */
          sysexit (EXIT_MEM);

   for (i=0;   i<=maxitems;   ++i)  m->orighost[i] = 0;

   RETURN (1);
}
