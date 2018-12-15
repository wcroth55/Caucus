
/*** SYSTIMEMS.  Returns absolute system time, in milliseconds,
/    since the epoch (Jan 1 1970). */

/*** Copyright (C) 2009 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA 11/28/09 New function. */

#include <stdio.h>
#include <sys/timeb.h>
#include "caucus.h"
#include "systype.h"

FUNCTION  long systimems() {

#if UNIX
   struct timeb tp;
   ENTRY  ("systimems", "");

   ftime  (&tp);
   RETURN (tp.time * 1000 + tp.millitm);
#endif

}
