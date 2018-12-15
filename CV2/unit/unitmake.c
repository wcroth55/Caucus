
/*** UNIT_MAKE.  Open the file of TYPE for write/create. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:54 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

extern struct unit_template  units[];
extern int                   debug;

FUNCTION  unit_make (n)
   int  n;
{
   char   backup[130];
   char   format[160];
   char  *bugtell();
   Chix   newstr;
   int    success;

   ENTRY ("unitmake", "");
   if (debug & DEB_FILE)  bugtell ("MAKE", n, L(0), "");

   newstr    = chxalloc (L(160), THIN, "unitmake newstr");
   success   = 0;

   /*** Bug check: is this file N already open? */
   if (units[n].access != LOCKED) {
      buglist (bugtell ("Unitmake: ", n, L(0), "file is not locked!\n"));
      FAIL;
   }

   /*** We can always write to the bit bucket. */
   if (n == XIBB)  SUCCEED;

#if UNIX | NUT40 | WNT40
   /*** Make up name of BACKUP file. */
   sysbackup (backup, units[n].nameis);

   if (n < XURG) {
      if (NOT unitmkseq (n, 1))   FAIL;
   }

   else {
      /*** For normal files, we're going to rewrite the entire file anyway,
      /    so just rename the original to the backup file. */
      if (syscheck (backup)) {
         sprintf (format, "%s\n  %d %s\n",
                 "Unitmake: seq: abort found, use backup.", 
                  n, units[n].nameis);
         buglist (format);
      }
      else if (syscheck (units[n].nameis) )
         sysrename (units[n].nameis, backup);

      if (NOT unitmkseq (n, 0))  FAIL;
   }
#endif

   units[n].access  = WRITE;
   units[n].written = 0;

   SUCCEED;

 done:

   chxfree ( newstr );

   RETURN ( success );
}
