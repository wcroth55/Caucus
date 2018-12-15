/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSRMDIR.  Completely remove directory PATH and all it's files. */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR  1/08/98 20:00 Make it recursively remove all files and subdirs. */

#include <stdio.h>
#include <errno.h>
#include "caucus.h"
#include "systype.h"

/*** Old style unixes, i.e. prior to new "dirent.h" */
#define  OLD    HPXA9

/*** Newer "dirent" forms. */
#define  DIRENT SYSV | BSD21 | SUN41 | LNX12 | NUT40 | FBS22

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

FUNCTION  sysrmdir (path)
   char   path[];
{

#if UNIX
   char             fname[1024];
   DIR             *dir;
   struct DIRENT_T *dp;

   ENTRY ("sysrmdir", "");

   /*** Make sure the directory really exists. */
   if (NOT sysisdir (path))   RETURN (0);

   if ( (dir = opendir (path)) == (DIR *) NULL)  RETURN (0);

   /*** Find and return the next file in the list. */
   while ( (dp = readdir (dir)) != (struct DIRENT_T *) NULL) {
      if (NOT strcmp (dp->d_name, ".") ||
          NOT strcmp (dp->d_name, ".."))    continue;
      strcpy (fname, path);
      strcat (fname, "/");
      strcat (fname, dp->d_name);

      /*** If we can't delete it, and it's a directory, recurse. */
      if (unlink (fname) < 0  &&  sysisdir (fname))  sysrmdir (fname);
   }

   closedir (dir);
   rmdir  (path);
   RETURN (1);
#endif



/*------------------------WNT40-----------------------------------*/
#if WNT40
   struct _finddata_t c_file;
   long               hfile;
   int                first;
   char               pattern[256], fullpath[512], fname[256];

   ENTRY ("sysrmdir", "");

   /*** Make sure the directory really exists. */
   if (NOT sysisdir (path))   RETURN (0);

   sprintf (pattern, "%s/*.*", path);
   for (first=1;   (1);   first = 0) {
      if (first) {
         if ( (hfile = _findfirst (pattern, &c_file)) == -1L)   RETURN (0);
      }
      else {
         if (_findnext (hfile, &c_file) != 0)   break;
      }
      strcpy    (fname, c_file.name);
      if (streq (fname, "." ))  continue;
      if (streq (fname, ".."))  continue;

      sprintf (fullpath, "%s/%s", path, fname);
      if (_unlink (fullpath) < 0  &&  sysisdir (fullpath))  sysrmdir (fullpath);
   }

   _findclose (hfile);
   _rmdir (path);
   RETURN (1);
#endif

}
