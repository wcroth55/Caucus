
/*** SYSFDOPEN.  Open a file and return a file descriptor.
/
/    fd = sysfdopen (name, write, perm);
/ 
/    Parameters:
/       char  *name;    (file name)
/       int    write;   (open for writing?)
/       int    perm;    (unix-style permission mask)
/
/    Purpose:
/       Open a file and return a file descriptor.
/       (*&^%! Microsoft had to use slightly different forms of the
/       standard C functions, so we have to "hide" the differences
/       in this function.)
/
/    Returns: file descriptor number on success
/             -1 on failure
/
/    Error Conditions:
/ 
/    Known bugs: PERM not used in WNT40 code
/
/    Home:  sweb/sysfdopen.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/09/98 23:52 New function. */

#include "systype.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#if WNT40
#include <io.h>
#endif


int sysfdopen (char *name, int write, int perm)
{

#if UNIX
   int    save_mask, fd;

   save_mask = umask((mode_t) 0);
   fd = open (name, (write ? O_WRONLY | O_CREAT | O_TRUNC : O_RDONLY), perm);
   umask ((mode_t) save_mask);
   return (fd);
#endif

#if WNT40
   return (_open (name, (write ? _O_WRONLY | _O_CREAT | _O_TRUNC : _O_RDONLY),
                        (write ? _S_IWRITE : _S_IREAD) ) );
   /*** Someday PERM should be used to set the file access rights... */
#endif

}
