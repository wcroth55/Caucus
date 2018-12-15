
/*** CML_COPYFILE.   Copy a file to the Caucus domain.
/
/    ok = cml_copyfile (orig, target, perms);
/   
/    Parameters:
/       int     ok;         (success?)
/       char   *orig;       (full pathname of "original" file)
/       char   *target;     (full pathname of "target" file)
/       int     perms;      (permission on copied file)
/
/    Purpose:
/       Copy a file from one place to another within a caucus "domain".
/
/    How it works: 
/
/    Returns: 1 on success, 
/            -1 if file in ORIG could not be opened
/             0 if TARGET could not be written
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home: sweb/cmlcopyf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/19/98 16:34 New function. */
/*: CR 08/28/02 sysunlink target file before writing */

#include <stdio.h>
#include "systype.h"
#include "chixuse.h"
#include "sweb.h"
#include "rwmacro.h"

#if UNIX | NUT40
#include <sys/types.h>
#include <fcntl.h>
#include "fcntl2.h"
#include <unistd.h>
#define  PERMS  0611
#endif

#if WNT40
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define  PERMS  _S_IWRITE | _S_IREAD
#endif

#define  BUFMAX  4096
#define  SLASH(x)   (x=='/' || x=='\\')


FUNCTION  cml_copyfile (char *orig, char *target, int perms)
{
   char  *pos;
   char   buf[BUFMAX+2];
   int    got, f_orig, f_libr, rc;
   ENTRY ("cml_copyfile", "");

   if (NULLSTR(orig)  ||  NULLSTR(target)) RETURN(0);
   if (perms == 0)  perms = 0644;

   /*** Create any sub-directories along the way. */
   for (pos=target+1;   *pos;   ++pos) {
      if (SLASH(*pos)) {
         *pos = '\0';
         rc = sysmkdir (target, 0711, NULL);
         *pos = '/';
      }
   }

   /*** Copy the file. */
   if ( (f_orig = _OPEN (orig, O_RDONLY | O_BINARY))                    < 0) 
      RETURN(-1); 

#if WNT40
   nt_mkfile (target, perms);
#endif
   sysunlink (target);
   if ( (f_libr = _OPEN3(target, O_WRONLY | O_BINARY | O_CREAT, PERMS)) < 0) { 
      _CLOSE  (f_orig);   RETURN(0); 
   }

   while ( (got = _READ (f_orig, buf, BUFMAX)) > 0)  _WRITE (f_libr, buf,  got);
   _CLOSE (f_libr);
   _CLOSE (f_orig);
#if UNIX | NUT40
   chmod (target, perms);
#endif

   RETURN (1);
}
