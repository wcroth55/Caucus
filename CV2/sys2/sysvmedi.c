
/*** SYSVMEDIT.   Access to VM/CMS full-screen editing. 
/
/    ok = sysvmedit (file, context);
/
/    Parameters:
/       int   file;       (unit number of file to append text to)
/       int   context;    (is there already text to be left on screen?)
/
/    Purpose:
/       Open a full-screen editing window (with wordwrap) for VM/CMS
/       systems.
/
/       For all other systems, this is a no-op.
/
/    How it works:
/
/    Returns: 1 on success
/             0 otherwise
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions: vmiedit()
/
/    Called by:  textenter()
/
/    Operating system dependencies: VM/CMS
/
/    Known bugs:
/
/    Home:  sys/sysvmedi.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  6/04/92 17:46 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "handicap.h"
#include "systype.h"

FUNCTION  sysvmedit (file, context)
   int    file, context;
{

#if UNIX | NUT40 | WNT40
   return (0);
#endif

}
