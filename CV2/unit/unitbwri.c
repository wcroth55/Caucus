
/*** UNIT_B_WRITE.  Write some data to a file.
/
/    ok = unit_b_write (n, buf, size);
/
/    Parameters:
/       int   ok;      (success?)
/       int   n;       (unit number of open file)
/       char *buf;     (use data from here)
/       int4  size;    (size of data to write)
/
/    Purpose:
/       Write a bufferful of data from BUF to the open file on unit N.
/       Use file format and/or record attribute info in units structure.
/
/       Unlike unit_write(), unit_b_write() does not adjust the data it
/       is writing in any way.
/
/    How it works:
/       
/    Returns: 1 on success
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
/       We may wish to have unit_b_write() return some indication
/       of an error other than the return value 0.
/
/    Home:  unit/unitbwri.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  3/05/93 13:39 Create this. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "handicap.h"
#include "systype.h"
#include "entryex.h"
#include "unitcode.h"
#include "units.h"

extern struct unit_template units[];

FUNCTION  unit_b_write  (n, buf, size)
   int   n;
   char *buf;
   int4  size;
{
   char *bugtell();

   ENTRY ("unit_b_write", "");

   if (n <= XITT) RETURN (0);    /* No writing binary to terminal yet! */
   if (n == XIBB) RETURN (1);    /* "OK, I did it!!!" */

   if (units[n].access != WRITE)
      buglist (bugtell ("Unit_b_write: ", n, L(0),
               "writing to unopened file!\n"));

#if UNIX | NUT40 | WNT40
   RETURN (_WRITE (units[n].fd, buf, (int)size));
#endif

}
