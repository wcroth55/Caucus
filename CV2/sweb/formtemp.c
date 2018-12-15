/*** FORM_TEMP.    Handle temporary files created from multi-part forms.
/
/    name = form_temp (create);
/ 
/    Parameters:
/       char   *name;    (returned name of new temp file)
/       int     action;  (1=create new temp file name;  0=delete all temp files)
/       int     create;  (create or delete file?)
/
/    Purpose:
/       Create and delete temp file names needed for handling
/       multi-part forms.
/
/    How it works:
/       If CREATE == 1, creates a new temp file name and returns it.
/       If CREATE == 0, deletes all temp files created.
/
/    Returns: new temp file name, or NULL.
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  formtemp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/06/96 20:26 New function, derived from old readform.c */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

#if UNIX
#include <unistd.h>
#endif

FUNCTION  char * form_temp (int create)
{
   static int  files = 0;
   static char filename[200];
   char        tmpdir[200];
   int         i;
   ENTRY ("form_temp", "");

   systmpdir (tmpdir, 0);

   if (create) {
      sprintf (filename, "%s/cau%06d.%04d", tmpdir, (int) getpid(), files++);
      RETURN  (filename);
   }

   for (i=0;   i<files;    ++i) {
      sprintf (filename, "%s/cau%06d.%04d", tmpdir, (int) getpid(), i);
      unlink  (filename);
   }
   RETURN (NULL);
}
