/*** GETMYHOST.  Get the name of my (the current) host. 
/
/    getmyhost (name);
/
/    Parameters:
/       Chix   name;    (Where to put the name of this host)
/
/    Purpose:
/       Get the name of this host (as known by the distributed
/       conferencing layer) and put it in NAME.
/
/    How it works:
/       The first time getmyhost() is called, it goes out the the
/       XSNF file and reads the current host name.  Thereafter,
/       it keeps the current host name stored in a static area.
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus (notably isauthor()).
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/getmyhos.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/19/91 14:26 New function. */
/*: JV  7/10/91 10:14 Added real code to call get_chn() */
/*: CR  7/30/91 13:32 Fix comments. */
/*: CR  9/17/91 17:14 Return empty host if not dist or getchn fails. */
/*: JX  5/13/92 16:25 Fix Flag use, chixify. */
/*: CR  1/26/93 15:49 Use PLAIN and strplain() to decrypt license info. */
/*: JV  9/26/93 13:29 ENTRY must be before other code. */

#include <stdio.h>
#include "unitcode.h"
#include "caucus.h"

FUNCTION  getmyhost (name)
   Chix   name;
{
   static Chix   host;
   static Flag   first_call;
   int success;

   ENTRY ("getmyhost", "");

   first_call = 1;

   host      = chxalloc (L(100), THIN, "getmyhost host");
   success   = 0;

   if (name == nullchix)  SUCCEED;

   chxclear(name);
   SUCCEED;

   if (first_call) {
      first_call = 0;
      if (NOT get_chn (host, XSNF, 0))  chxclear(host);
   }

   chxcpy (name, host);

   SUCCEED;

 done:

   chxfree ( host );

   RETURN ( success );
}
