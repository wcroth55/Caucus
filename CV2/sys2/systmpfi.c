
/*** SYSTMPFILE.  Build a full pathname for a temporary file.
/
/    ok = systmpfile (dir, file);
/
/    Parameters:
/       int     ok;        (success?)
/       Chix    dir;       (directory for file)
/       Chix    file;      (full pathname)
/
/    Purpose:
/       Build a full pathname for a new, temporary file.  Typically this
/       should be created in a large, publicly accessible pool of disk
/       space, even though we will create private files there.  DIR allows
/       the caller to specify a directory to put the file, or NULL to let
/       systmpfile() choose the directory.
/
/    How it works:
/       On systems which supply a system call to build such a pathname,
/       use it.  Otherwise, make one up.
/
/    Returns: 1 if a pathname was constructed correctly
/             0 otherwise
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/       Plenty.
/
/    Known bugs:      none
/
/    Home:  sys/systmpfi.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  4/13/93 13:04 Created function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "systype.h"

FUNCTION  systmpfile (dir, file)
   Chix  dir, file;
{

#if UNIX
   char *tmpname;

   if (dir==nullchix  ||  EMPTYCHX (dir))
        tmpname = tempnam ("/tmp",        (char *) NULL);
   else tmpname = tempnam (ascquick(dir), (char *) NULL);

   if (tmpname != NULL) {
      chxofascii (file, tmpname);
      free (tmpname);
   }
   else chxofascii (file, "/tmp/caucus_err");
#endif

#if NUT40 | WNT40
   static char tdir[128], tfile[256];

   if (dir==nullchix  ||  EMPTYCHX (dir))
        systmpdir (tdir);
   else strcpy    (tdir, ascquick(dir));

   sprintf    (tfile, "%s/%s", tdir, tmpnam (NULL));
   chxofascii (file, tfile);
#endif

   return  (1);
}
