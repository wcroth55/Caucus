
/*** NT_CHKFILE.  Windows/NT only: if a file does not exist, create it.
/
/    nt_chkfile (name, perms);
/   
/    Parameters:
/       char  *name;     (name of file to be created)
/       int    perms;    (permissions in Unix format)
/
/    Purpose:
/       If file NAME does not already exist, create it using the 
/       traditional Unix file permission mask.
/
/    How it works:
/       Unix: no-op.
/       NT:   calls syscheck() and nt_mkfile().
/
/    Returns:
/
/    Known bugs:
/
/    Home:  nkchkfile.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/23/97 15:31 New function. */

#include "systype.h"
#define   FUNCTION
#define   NOT    !

FUNCTION nt_chkfile (char *name, int perms)
{

#if WNT40
   if (NOT syscheck (name))  nt_mkfile (name, perms);
#endif

   return (1);
}
