
/*** UNIT_B_MAKE.  Create a file for binary writing.
/
/    ok = unit_b_make (n, format, rec_len, user);
/
/    Parameters:
/       int  ok;       (success in creating file)
/       int  n;        (unit number of file)
/       int4 format;   (file format options)
/       int  rec_len;  (record length)
/       int  user;     (1=create as user, 0=create as Caucus)
/
/    Purpose:
/       Create and open a binary file.
/
/    How it works:
/    unit_b_make() creates and opens the binary file associated with
/    unit N.  If USER is 1, it tries to create the file as
/    though the user were writing it.  Otherwise, it creates the file
/    with Caucus' privileges. 
/
/    Returns: 1 on success
/             0 on failure (error)
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: 
/       Unix: uses open()
/       DOS:  open()
/       VV:   not implemented.
/
/    Known bugs:
/
/    Home:  unit/unitbmak.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  3/03/93 14:40 Create this. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

#include <fcntl.h>

#if WNT40
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#endif

#if SUN41
#include <sys/stat.h>
#endif

extern struct unit_template units[];
extern int                  debug;

FUNCTION  unit_b_make (n, format, rec_len, user)
   int    n, rec_len, user;
   int4   format;
{

#if UNIX | NUT40 | WNT40
   int    oldmask;

   ENTRY ("unit_b_make", "");

   if (user)  sysprv(0);
   if (NOT sysaccess (units[n].nameis))  units[n].fd = -1;
   else {
#if UNIX
      if (n==XIRE)  oldmask = umask ((mode_t) 0077);
#endif
      unlink (units[n].nameis);

#if WNT40
      units[n].fd = _open(units[n].nameis, O_WRONLY | O_CREAT | O_BINARY, 
                                          _S_IWRITE);
#endif
#if NUT40
      units[n].fd = open (units[n].nameis, O_WRONLY | O_CREAT | O_BINARY, 0700);
#endif
#if UNIX
      units[n].fd = open (units[n].nameis, O_WRONLY | O_CREAT, 0700);
#endif
      units[n].fp = NULL;
#if UNIX
      if (n==XIRE)  umask ((mode_t) oldmask);
#endif
   }
   if (user)  sysprv(1);

   if (units[n].fd <= 0)  RETURN (0);
   units[n].access = WRITE;
   RETURN (1);
#endif

}
