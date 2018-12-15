
/*** SYSMKHOST.  Make (create) a host directory.
/
/    Make the host directory for host number N.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV 12/10/91 18:08 Create this. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include "caucus.h"
#include "systype.h"
#include <stdio.h>

/*ARGSUSED*/

FUNCTION  sysmkhost (n)
   int    n;
{

#if UNIX | NUT40 | WNT40
   char   name[80];

   ENTRY ("sysmkhost", "");

   unit_name (name, XHDD, L(n), L(0), L(0), L(0), nullchix);
   sysmkdir  (name, 0700, NULL);
   RETURN (1);
#endif

}
