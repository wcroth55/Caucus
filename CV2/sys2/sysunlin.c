
/*** SYSUNLINK.  System-dependent "delete file" procedure. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:46 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern int debug;

FUNCTION  sysunlink (file)
   char file[];
{
   int  code;

#if UNIX | NUT40
   ENTRY ("sysunlink", "");
   unlink (file);
   code = 1;

   RETURN (code);
#endif


#if WNT40
   return (_unlink (file) == 0);
#endif

}
