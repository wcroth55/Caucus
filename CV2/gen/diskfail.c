
/*** DISK_FAILURE.   Report (or test for) a disk-write failure.
/
/    occurred = disk_failure (set, error, file);
/
/    Parameters:
/       int    occurred;     (has a disk-write failure occurred?)
/       int   *error;        (error code, typically errno)
/       char  *filename;     (name of file error occurred on)
/
/    Purpose:
/       Report, and test for, disk-write failure in Caucus.
/       Under certain (unknown) circumstances, Caucus creates
/       an empty data file, and then cannot write any more data to it.
/       Disk_failure() is used to report this error, and then test for
/       its occurrence at higher levels in the application program.
/          
/    How it works:
/       disk_failure (1, &error, file_name) 'sets' and remembers an
/       error condition.  Error should be a copy of the momentary value
/       of errno, and file_name the relevant file name.
/
/       In this case, disk_failure() also attempts to append error
/       information to the bugslist file; if that fails, it tries to
/       append the information to /tmp/caucus.err.
/
/       disk_failure (0, &error, file_name) returns 1 if an error has
/       occurred, setting error and file_name; returns 0 if no error.
/
/    Returns: 1 if a disk-write failure occurred
/             0 otherwise
/
/    Known bugs:
/
/    Home:  gen/diskfail.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/26/97 13:44 New function. */

#include <stdio.h>
#include "caucus.h"
#include "units.h"

extern struct unit_template  units[];

FUNCTION  disk_failure (int set, int *error, char *file)
{
   static int  occurred  = 0;
   static int  error_num = 0;
   static char filename[256];
   char        temp[512];
   int         year, month, day, hour, minute, sec, rc, ok, wday;
   int4        now;
   FILE       *fp;

   if (set) {
      occurred  = 1;
      error_num = *error;
      strcpy (filename, file);

      now = systime();
      sysymd  (now, &year, &month, &day, &hour, &minute, &sec, &wday);
      sprintf (temp, "%4d %2d %2d %02d:%02d: disk failure %d\n  file: %s\n",
                     year, month, day, hour, minute, error_num, filename);

      ok = 0;
      if (unit_lock (XSBG, WRITE, L(0), L(0), L(0), L(0), nullchix)) {
         nt_chkfile (units[XSBG].nameis, 0700);
         if ( (fp = fopen (units[XSBG].nameis, "a")) != NULL) {
            fprintf (fp, "%s", temp);
            rc = fclose (fp);
            ok = (rc == 0);
         }
         unit_unlk (XSBG);
      }
      if (NOT ok) {
         nt_chkfile ("/tmp/caucus.err", 0700);
         if ( (fp = fopen ("/tmp/caucus.err", "a")) != NULL) {
            fprintf (fp, "%s", temp);
            fclose (fp);
         }
      }

      return (1);
   }

   else if (occurred) {
      *error = error_num;
      if (file != NULL)  strcpy (file, filename);
      return (1);
   }

   else return (0);
}
