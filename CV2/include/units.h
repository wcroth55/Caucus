/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** UNITS.H  Definition of structure & other items used by UNIT_ routines.
/
/    Copyright (C) 1987 Camber-Roth */

/*: AA  7/29/88 17:47 Source code master starting point */
/*: CR  9/30/88 15:17 Add server code in "#if SRV" section. */
/*: CR  6/21/90 11:38 Move LSYS, LUSR, LCNF defs to handicap.h. */
/*: CR  9/27/90 12:27 Remove unused LOCKIT and UNLOCK macros. */
/*: CR  9/04/91 16:18 Expand locktype_t definition. */
/*: UL 12/11/91 18:54 Expand locktype_t.nconf to long. */
/*: CX  5/12/92 16:44 Add unit control codes. */
/*: CR  6/24/92 10:54 Expand size of nameis. */
/*: CI 10/04/92 23:31 Chix Integration. */
/*: CR 10/30/92 14:56 Add UNIT_SETSUB, GETSUB. */
/*: JV  3/05/93 10:26 Add format, record length to unit_template. */

#include "systype.h"
#include "int4.h"

#if WNT40
#include <windows.h>
#include <iostream.h>
#endif

#ifndef UNITS_H
#define UNITS_H

struct unit_template {
   FILE  *fp;                 /* File pointer for open file of this type */
   int   fd;                  /* File descriptor if we need one. */
   short item;                /* Item number file is associated with. */
   short access;              /* READ, WRITE, or APPEND? */
   char  nameis[160];         /* Name of file, created by UNIT_LOCK. */
   int4  locknum;             /* Locking record number. */
   short locktype;            /* Locking type (for Unix). */
   int4  format;              /* File format, if any. */
   int   rec_len;             /* Record length */
   long  written;             /* bytes written (for debugging) */
#if SRV
   char *buffer;              /* Buffer containing text sent by server */
   char *p;                   /* Pointer to what we've read in the buffer. */
   short buflen;              /* Number of chars in the buffer. */
#endif
};

/*** Locking-type structure for Unix systems. */
struct locktype_t {
   int    fd;            /* File descriptor # of open lockmap file. */
   int4   nconf;         /* Conf/host/rec bay/ship bay #. */
   char   user[24];      /* ...or userid. */
   char   file[256];     /* Full pathname of lockmap file. */
#if WNT40
   HANDLE handle;        /* File handle (instead of fd). */
#endif
};

/*** Unit_control codes. */
#define UNIT_GET     0x1000     /* Get the control code for this unit. */
#define UNIT_SET     0x2000     /* Set the control code for this unit. */
#define UNIT_GETSUB  0x3000     /* What unit subsitutes for this unit? */
#define UNIT_SETSUB  0x4000     /* Substitute another unit number. */
#define UNIT_MASK    0x0FFF     /* Mask for code values. */


#endif
