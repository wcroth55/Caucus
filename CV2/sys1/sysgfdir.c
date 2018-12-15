
/*** SYSGFDIR.  Get the file names in a directory, one at a time.
/
/    ok = sysgfdir (dname, fname, start, error);
/
/    Parameters:
/       int    ok;      (success?)
/       char  *fname;   (next file in dir or NULL)
/       char  *dname;   (full pathname of the directory)
/       int    start;   (start at beginning of a listing? stop?)
/       int   *error;   (error code if failure)
/
/    Purpose:
/       Intended to be used in a loop to check all the filenames in a
/       directory.  First use of this will be to calculate the size of a dir
/       (and therefor a conference).
/
/    How it works:
/       START has three legitimate values:
/       1  Start at the beginning of the directory listing
/       0  Continue the listing from the last call
/       2  Stop this listing now (presumably before the end)
/
/       Notes: caller *must* search to the end or call this with start==2
/
/    Returns: 1 on success, 0 on failure, 2 on EOD (end-of-dir, no file)
/
/    Error Conditions:  (see syserror.h.)
/       SE_NOTIMP     Function not implemented for this system.
/       SE_BADDIR     File DNAME does not exist.
/       SE_NOINFO     Cannot get information for unknown reason.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  application.
/
/    Operating system dependencies: considerable!
/
/    Known bugs:      none
/
/    Home:  sys/sysgfdir.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  3/28/93 12:00 Create this. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"
#include "syserror.h"

extern Chix    confid;

/*** Old style unixes, i.e. prior to new "dirent.h" */
#define  OLD    HPXA9

/*** Newer "dirent" forms. */
#define  DIRENT SYSV | BSD21 | SUN41 | LNX12 | NUT40 | FBS22
/* NOTRAN -- win32 code not translated yet */

#if OLD
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#define DIRENT_T direct
#endif

#if DIRENT
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#define DIRENT_T dirent
#endif

#if WNT40
#include <io.h>
#endif

FUNCTION  sysgfdir (dname, fname, start, error)
   char  *dname, *fname;
   int    start, *error;
{

/*------------------------UNIX------------------------------------*/
#if UNIX | NUT40

   static DIR    *dir;
   struct DIRENT_T *dp;

   ENTRY ("sysgfdir", "");

   if (start == 1) {
      
      /*** Make sure the directory really exists. */
      *error = 0;
      if (NOT sysisdir (dname)) {
         *error = (SE_BADDIR);
         RETURN (0);
      }

      if ( (dir = opendir (dname)) == (DIR *) NULL)  
         { *error = SE_BADDIR;      RETURN (0);}
   }

   /*** If caller is done prematurely, close the dir and return. */
   if (start == 2) {
      closedir (dir);
      RETURN (1);
   }

   /*** Find and return the next file in the list. */
   while ( (dp = readdir (dir)) != (struct DIRENT_T *) NULL) {
      if (NOT strcmp (dp->d_name, ".") ||
          NOT strcmp (dp->d_name, ".."))    continue;
      strcpy (fname, dp->d_name);
      RETURN (1);
   }

   /*** At this point there wasn't another file in the list. */
   closedir (dir);
   RETURN (2);
#endif


/*------------------------WNT40-----------------------------------*/
#if WNT40
   static struct _finddata_t c_file;
   static long               hfile;
   char                      pattern[256];

   ENTRY ("sysgfdir", "");

   if (start == 1) {

      /*** Make sure the directory really exists. */
      if (NOT sysisdir (dname))
         { *error = SE_BADDIR;      RETURN (0);}

      sprintf (pattern, "%s/*.*", dname);
      if ( (hfile = _findfirst (pattern, &c_file)) == -1L)   RETURN (2);
      strcpy  (fname, c_file.name);
      if (NOT (streq (fname, ".") || streq (fname, "..")))   RETURN (1);
   }

   /*** If caller is done prematurely, close the dir and return. */
   if (start == 2) {
      _findclose (hfile);
      RETURN (1);
   }

   /*** Find and return the next file in the list. */
   while (_findnext (hfile, &c_file) == 0) {
      strcpy  (fname, c_file.name);
      if (NOT (streq (fname, ".") || streq (fname, "..")))   RETURN (1);
   }

   /*** At this point there wasn't another file in the list. */
   _findclose (hfile);
   RETURN (2);
#endif

}
