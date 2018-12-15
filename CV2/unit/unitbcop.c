
/*** UNIT_B_COPY.   Copy unit SRC to unit DEST.
/
/    ok = unit_b_copy (src, dest);
/
/    Parameters:
/      int  ok;    (success?)
/      int  src;   (source file unit number, locked by caller)
/      int  dest;  (destination file unit number, locked by caller)
/      
/    Purpose:
/      This function copies one file to another, retaining any
/      file format and record length attributes.   If there was
/      a pre-existing "destination" file, it is deleted.
/
/      It DOES:
/         copy binary data
/         retain file format and record length
/         copy from: Caucus file to:    Caucus file
/                    Caucus file        User file  (unit XIRE)
/                    User file (XIRE)   Caucus file
/                    
/      It does NOT:
/         allow variable length records (unless data can be moved
/            independently of record structure)
/
/    How it works:
/      Both units must already be locked.
/
/    Returns 1 on success, 0 on failure.
/
/    Error Conditions:
/
/    Side effects:
/
/    Related Functions:
/
/    Called by:
/
/    Operating System Dependencies:
/
/    Known Bugs: Vax/VMS code not written.
/
/    Home: unit/unitbcop.c
/
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  3/05/93 10:45 Create this. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR  4/11/03 Use <errno.h> */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"

#if UNIX | NUT40 | WNT40
#include <signal.h>
#include <errno.h>
#endif

extern struct unit_template  units[];

FUNCTION  unit_b_copy (src, dest)
   int src, dest;
{

/*----------------------------------Unix, DOS-------------------------------*/
#if UNIX | NUT40 | WNT40
   char     buf[1024];
   int      len;

   if (NOT unit_b_view (src,  L(0), 0))                          return (0);
   if (NOT unit_b_make (dest, L(0), 0, dest==XIRE ? 1 : 0))
                                        { unit_close (src);    return (0); }

   for (len=1024;   (len = read (units[src].fd, buf, len)) > 0;)
      write (units[dest].fd, buf, len);

   unit_close (src);
   unit_close (dest);
#endif

   return (1);
}
