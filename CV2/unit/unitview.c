
/*** UNIT_VIEW.  Open the file of TYPE for read only.
/
/    Returns 1 on success, 0 on failure.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:55 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR  4/11/03 Use <errno.h> */

#include <stdio.h>
#include <errno.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

extern struct unit_template  units[];
extern int                   debug;

FUNCTION  unit_view (n)
   int  n;
{
   char  backup[80];
   char *sysmem();
   int   file_exists, bkup_exists;
   char  format[160];
   char *bugtell();

   ENTRY ("unitview", "");
 
   if (debug & DEB_FILE)
      bugtell ("VIEW", n, (int4) units[n].access, units[n].nameis);

   if (units[n].access != LOCKED) {
      if (units[n].access == READ)
         buglist (bugtell ("Unitview: ", n, L(0), "file already open\n"));
      else
         buglist (bugtell ("Unitview: ", n, L(0), "file not locked!\n"));
      RETURN  (0);
   }


#if UNIX | NUT40 | WNT40

   /*** Ensure Caucus privilege is not used for uncontrolled file access. */
   if (n < XURG)  sysprv(0);
   if (NOT sysaccess (units[n].nameis))  {
      if (n < XURG)  sysprv(1);
      RETURN (0);
   }

   /*** If a backup file exists, something aborted while writing a file.
   /    Restore the backup to its proper place, and continue. */
   sysbackup (backup, units[n].nameis);
   bkup_exists = syscheck (backup);
   file_exists = (bkup_exists ? syscheck (units[n].nameis) : 1);

   if (bkup_exists) {
      if (n < XURG)  sysprv(1);
      sprintf (format, "%s\n  %d '%s'\n",
        "Unitview: abort found, backup file used.", n, units[n].nameis);
      buglist (format);
      if (file_exists) {
         sprintf (format, "%s\n    %d %s\n",
            "Unitview: abort addendum: original file found: ", n,
            units[n].nameis);
         buglist (format);
      }
      if (n < XURG)  sysprv(0);
      sysunlink (units[n].nameis);
      sysrename (backup, units[n].nameis);
   }

#if UNIX | NUT40
   units[n].fp = fopen (units[n].nameis, "r");
#endif
#if WNT40
   units[n].fp = fopen (units[n].nameis, "rt");
#endif

   units[n].fd = -1;
   if (n < XURG)  sysprv(1);
   if (units[n].fp == NULL) {
      if (debug & DEB_FILE) {
         bugtell ("unitview: open failure", errno, L(0), units[n].nameis);
      }
      RETURN (0);
   }
#endif
 
   units[n].access = READ;
   RETURN (1);
}
