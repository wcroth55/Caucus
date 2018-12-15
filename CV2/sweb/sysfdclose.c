
/*** SYSFDCLOSE.  Close a file opened with sysfdopen().
/
/    sysfdclose (fd);
/ 
/    Parameters:
/       int    fd;      (file descriptor)
/
/    Purpose:
/       Close a file opened with sysfdopen().
/
/    Returns: 
/
/    Error Conditions:
/ 
/    Known bugs: 
/
/    Home:  sweb/sysfdclose.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/09/98 23:52 New function. */

#include "systype.h"

#if WNT40
#include <io.h>
#endif

void sysfdclose (int fd)
{

#if UNIX
   close (fd);
#endif

#if WNT40
   _close (fd);
#endif

}
