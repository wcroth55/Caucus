
/*** SYSTYPE.    Return the "type" of operating system in use.
/
/    type = systype();
/
/    Parameters:
/       int   type;       (returned type of operating system)
/
/    Purpose:
/       Return type of operating system in use.  This allows 
/       higher-level, system-independent functions to make choices
/       depending on the operating system type.
/
/    How it works:
/
/    Returns: SYSTYPE_VMCMS   for VM/CMS
/             SYSTYPE_OTHER   for all other systems
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies: Yes!
/
/    Known bugs:
/
/    Home:  sys/systype.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  6/05/92 13:43 New function. */

#include <stdio.h>
#include "caucus.h"
#include "handicap.h"
#include "systype.h"

FUNCTION  systype()
{

#if UNIX | NUT40 | WNT40
   return (SYSTYPE_OTHER);
#endif

}
