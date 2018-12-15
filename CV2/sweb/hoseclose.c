
/*** HOSE_CLOSE.   Close a Hose.
/
/    num = hose_close (fh);
/ 
/    Parameters:
/       Hose   fh;          (file or pipe handle or descriptor)
/
/    Purpose:
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  sweb/hoseclose.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/11/97 17:16 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

#if WNT40
#include <windows.h>
#include <iostream.h>
#endif

FUNCTION  void hose_close (Hose fh)
{

#if UNIX | NUT40
   close (fh);
#endif

#if WNT40
   CloseHandle (fh);
#endif

}
