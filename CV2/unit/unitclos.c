
/*** UNIT_CLOSE.  Close a file opened with UNIT_MAKE or UNIT_VIEW. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:54 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"
#include "rwmacro.h"
#include <errno.h>

extern struct unit_template  units[];
extern char                 *unittag[];
extern int                   debug;

FUNCTION  unit_close (n)
   int  n;
{
   char  backup[80];
   char *bugtell();
   int   success = 0;
   int   rc;

   ENTRY ("unitclose", "");
   if (debug & DEB_FILE)
      bugtell ("CLOSE", n, (int4) units[n].access, (n>=0 ? unittag[n] : ""));

   if (n <= XITT  ||  n == XIBB)    SUCCEED;

   /*** Bug check: shouldn't be closing it if it wasn't open. */
   if (units[n].access != WRITE  &&  units[n].access != READ)
      buglist (bugtell ("Unitclos: ", n, L(0), "file is not open!\n"));

#if UNIX | NUT40 | WNT40
   if (units[n].fp != NULL)  rc = fclose (units[n].fp);
   if (units[n].fd !=   -1)  rc = _CLOSE (units[n].fd);
   units[n].fp = NULL; 
   units[n].fd =   -1;
   if (rc != 0) {
      int errnum = errno;
      disk_failure (1, &errnum, units[n].nameis);
      units[n].access = LOCKED;
      FAIL;
   }

   /*** Attempt to detect writing 0 bytes to data files for which
   /    this is always an error! */
   if (n==XURG || n==XUPA || n==XCMA || n==XCRF) {
      if (units[n].access == WRITE  &&  units[n].written == 0) {
         char temp[512];
         Chix id;
         id = chxalloc (20L, THIN, "unit_clos id");
         sysuserid (id);
         sprintf (temp, "%s\n  id='%s'  unit=%d   file='%s'\n",
            "Attempting to write 0 byte data:", ascquick(id), n, 
            units[n].nameis);
         buglist (temp);
         unitstack();
         chxfree (id);
         units[n].access = READ;   /* to force save of backup file */
      }
   }

   /*** If this closes a unit_make, delete the backup file, now that the
   /    current file is completely written out and finished with. */
   if (units[n].access == WRITE) {
      sysbackup (backup, units[n].nameis);
      sysunlink (backup);
   }
#endif

   units[n].access = LOCKED;
   SUCCEED;

 done:

   RETURN ( success );
}
