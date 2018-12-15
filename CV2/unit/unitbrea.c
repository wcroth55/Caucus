
/*** UNIT_B_READ.  Read some data from a file.
/
/    bytes = unit_b_read (n, buf, size);
/
/    Parameters:
/       int   bytes;   (# of bytes read: 0 on error, failure, eof, empty file)
/       int   n;       (unit number of open file)
/       char *buf;     (read data into here)
/       int4  size;    (max size of data to read)
/
/    Purpose:
/       Read a bufferful of data from the open file on unit N into BUF.
/       Use file format and/or record attribute info in units structure.
/
/       Unlike unit_read(), unit_b_read() does not interpret the data it
/       is reading in any way.
/
/    How it works:
/       
/    Returns: number of bytes read
/             0 on end-of-file, interrupt, hangup, or error
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: lots
/
/    Known bugs:
/
/    Home:  unit/unitbrea.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  3/05/93 13:29 Create this. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "handicap.h"
#include "systype.h"
#include "unitcode.h"
#include "units.h"
#include "entryex.h"
#include "rwmacro.h"

extern struct unit_template units[];

FUNCTION  unit_b_read  (n, buf, size)
   int   n;
   char *buf;
   int4  size;
{
   char *bugtell();
   int   bytes;

   ENTRY ("unit_b_read", "");

   if (n <= XITT) RETURN (0);      /* No reading binary from Keyboard! */
   if (n == XIBB) RETURN (0);      /* Bit Bucket always returns EOF */
   if (units[n].access != READ)
      buglist (bugtell ("Unitbread: ", n, L(0), "reading from unopened file!\n"));

#if UNIX | NUT40 | WNT40
   bytes = _READ (units[n].fd, buf, (int)size);
   if (bytes < 0)  bytes = 0;
   RETURN (bytes);
#endif

}
