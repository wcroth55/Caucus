/*** SYSACCESS.  Do we have access to the directory containing file NAME?
/
/    i = sysaccess (name);
/
/    Parameters:
/       int   i;
/       char *name;
/
/    Purpose:
/       Determine if we can read and write files in the directory that
/       contains file NAME.
/
/       Sysaccess() is called by unit_view(), unitmkseq(), and unit_append,
/       just before they start working with a particular file.
/
/    How it works:
/       For most systems, sysaccess() just returns 1.
/       For Ultrix, sysaccess() checks the usegid flag set by sysprv().
/       If usegid is off, we call access() to see if we can access
/       the directory with the real userid and group-id.
/
/    Returns: 1 on success (we can read/write in the directory)
/             0 otherwise
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions: sysprv()
/
/    Called by:  unit_view(), unitmkseq(), unit_append()
/
/    Operating system dependencies:
/       meaningful only for ultrix
/
/    Known bugs:
/
/    Home:  sys/sysacces.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/23/91 15:45 New function. */
/*: DE  6/01/92 15:19 Unchixified ( Reverted ) */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern int debug;

FUNCTION  sysaccess (name)
   char  *name;
{
   ENTRY ("sysaccess", "");

   RETURN (1);
}
