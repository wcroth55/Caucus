/*** UNIT_B_VIEW.  Open the file of TYPE for binary reading.
/
/    ok = unit_b_view (unit, format, reclen)
/
/    Parameters:
/      int unit;        unit number
/      int4 format;     file format, if any
/      int reclen;      record length, if any
/
/    Purpose:
/      This is the binary version of unit_view().  It opens a file
/      (which may contain non-ASCII data) for reading.  Reading should
/      be done with unit_b_read().
/
/      For some operating systems, unit_b_view() also checks the file format
/      and record attributes of the file being opened.  If the file is
/      formatted, and/or there are special attributes, this information must be
/      stored in the UNITS structure.
/
/   How it works:
/      Either/both of FORMAT and RECLEN can be zero, which means "use the
/      format and record length which already exist for the file."
/      Actually, these two args are only provided for stupid OS's that don't
/      allow you to look up this information.
/
/      Unix: calls open()
/      DOS:  ?
/      VMS:  ?
/
/   Returns 1 on success, 0 on failure.
/
/   Error Conditions:
/
/   Side effects:
/
/   Related Functions: unit_b_read()
/                      unit_close()
/
/   Called by: 
/
/   Operating System Dependencies:
/      Unix uses the open() call.  No support for file formats or record
/         attributes exists, or is needed.
/      VMS: needs support for formats and attributes.  The code here does *not*
/         work!
/      DOS: ?  Code not attempted.
/
/   Known Bugs: none
/
/   Home: unit/unitbvie.c
/
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  3/04/93 17:22 Create this for file attachments. */
/*: CP  8/16/93 16:06 Clean up fd/fp use, fix IM/PN system type. */
/*: CR  4/11/03 Use <errno.h> */

#include <stdio.h>
#include "handicap.h"
#include "debug.h"
#include "attach.h"
#include "systype.h"
#include "unitcode.h"
#include "units.h"

#if UNIX
#include <fcntl.h>
#include <errno.h>
#endif

#if VV
#include <errno>
#endif

extern struct unit_template  units[];
extern int                   debug;

FUNCTION  unit_b_view (n, format, reclen)
   int  n, reclen;
   int4 format;
{
   char  backup[80];
   int   file_exists, bkup_exists;
   char  tempstr[160];
   char *bugtell();

   ENTRY ("unit_b_view", "");
 
   if (debug & DEB_FILE)
      bugtell ("OPEN", n, (int4) units[n].access, units[n].nameis);

   if (units[n].access != LOCKED) {
      if (units[n].access == READ)
         buglist (bugtell ("Unit_b_view: ", n, L(0), "file already open\n"));
      else
         buglist (bugtell ("Unit_b_view: ", n, L(0), "file not locked!\n"));
      RETURN  (0);
   }

#if UNIX | IM | JM | PN | PR | VV | PX | PS

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
      sprintf (tempstr, "%s\n  %d '%s'\n",
        "Unit_b_view: abort found, backup file used.", n, units[n].nameis);
      buglist (tempstr);
      if (file_exists) {
         sprintf (tempstr, "%s\n    %d %s\n",
            "Unit_b_view: abort addendum: original file found: ", n,
            units[n].nameis);
         buglist (tempstr);
      }
      if (n < XURG)  sysprv(0);
      sysunlink (units[n].nameis);
      sysrename (backup, units[n].nameis);
   }

/*-----------------------FORTUNE & MASSCOMP & A3 | LA-------------------------*/
#if UNIX | PX | PS
   units[n].fd = open (units[n].nameis, O_RDONLY);
   units[n].fp = NULL;
   if (n < XURG)  sysprv(1);
   if (units[n].fd == -1) {
      if (debug & DEB_FILE) {
         bugtell ("unit_b_view: open failure", errno, L(0), units[n].nameis);
      }
      RETURN (0);
   }
#endif
 

/*---------------------------------------Windows/NT----------------------*/
#if WNT40
   5 = ;
   /*** Currently this function does not need to be implemented under
   /    Windows/NT, as it is only used by the text-interface "file
   /    attachment feature.  But this compile-time-error line is left
   /    here as a flag in case it does become needed in the future.
   /    For now, just delete the "5 = ;" line.
   /    (Charles Roth, 7/1/98)  */
#endif

/*---------------------------------------IBM PC--------------------------*/
#if IM | PN
#if IM
   units[n].fd = open  (units[n].nameis, O_BINARY | O_RDONLY);
#endif
#if PN
   units[n].fd = sopen (units[n].nameis, O_BINARY | O_RDONLY, SH_DENYNO);
#endif
   units[n].fp = NULL;
   if (n < XURG)  sysprv(1);
   if (units[n].fd == -1) {
      if (debug)
         bugtell ("unit_b_view: open failure", 0, L(0), units[n].nameis);
      RETURN (0);
   }
#endif

/*---------------------------------------Vax/VMS-------------------------*/
#if VV
  5 = ;
#endif


/*-----------------------NEW PRIME--------------------------------------*/
#if PR
   {
      fortran tsrc$$();
      extern  struct buf_template bufhead;
      struct  buf_template *p, *q;
      short   key, funit, type, code, chrpos[2];

      /*** Open the file with tsrc$$. */
      key = KREAD + KGETU;
      chrpos[0] = 0;
      chrpos[1] = strlen(units[n].nameis);
      tsrc$$ (&key, (char []) units[n].nameis, &funit, chrpos, &type, &code);
      if (n < XURG)   sysprv(1);
      if (code != 0)  RETURN(0);
      units[n].fd = funit;

      /*** Allocate a read buffer for this file, and initialize it. */
      q = (struct buf_template *) sysmem (sizeof(bufhead), "bufhead");
      q->next   = NULL;
      q->buf    = sysmem (258, "258");
      q->buflen = 0;
      q->bufptr = 0;
      q->n      = n;

      /*** Find the end of the linked list of read buffers,
      /    and attach this read buffer to the end of the list. */
      for (p=&bufhead;   p->next != NULL;   p = p->next) ;
      p->next   = q;
   }
#endif
#endif

   units[n].access  = READ;
   units[n].format  = format;
   units[n].rec_len = reclen;
   RETURN (1);
}

