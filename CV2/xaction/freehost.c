/*** FREE_HOST.   Deallocate a HOST.
/
/    free_host(h);
/
/    Parameters:
/       HOSTPTR  h;     (HOST (struct hostinfo_t))
/
/    Purpose:
/       Deallocate all memory which is part of a HOST.
/
/    How it works:
/       free_host() deallocates all memory associated with a host.
/       This includes memory allocated with make_host() AND any memory
/       allocated with make_hostinfo().  free_host() does not mind if
/       a hostinfo structure was never allocated.
/
/       There IS NO "free_hostinfo()" call.
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:  make_host()  make_hostinfo()
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  xaction/freehost.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  7/24/92 15:46 New function. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "xfp.h"

FUNCTION  free_host (h)
   HOSTPTR h;
{
   ENTRY ("free_host", "");

   if (h->chn != nullchix)    chxfree (h->chn);
   if (h->lhname != nullchix) chxfree (h->lhname);
   if (h->info != (Hinfoptr) NULL) {
      if (h->info->mgrid != nullchix) chxfree (h->info->mgrid);
      sysfree ((char *) h->info);
   }

   sysfree ((char *) h);

   RETURN (1);
}
