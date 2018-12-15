/*** CUS_DTEXT.   Customize conference text (greeting or introduction)
/
/    ok = cus_dtext (cnum, which, ctext, error);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    cnum;        (conference number)
/       int    which;       (CUS_GREET or CUS_INTRO)
/       Chix   ctext;       (complete text)
/       int   *error;       (return error code)
/
/    Purpose:
/       Write text in CTEXT to conference text file selected by
/       conference number CNUM and text file type WHICH.
/
/    How it works:
/
/    Returns: 1 on success, sets ERROR to 0.
/             0 on any error condition, see below.
/
/    Error Conditions: sets ERROR accordingly:
/       DBADACT     bad WHICH code
/       DNOCONF     cannot lock or open; must be wrong conf?
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions, XFU
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/cusdtext.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/28/91 15:15 New function. */
/*: CR  7/30/91  0:48 Use return value from ascofchx() more carefully. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JX  5/13/92 16:02 Remove unnecessary I, MORE, SUCCESS. */
/*: CL 12/10/92 17:06 Long ascofchx args. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"
#include "derr.h"

FUNCTION  cus_dtext (cnum, which, ctext, error)
   int    cnum, which;
   Chix   ctext;
   int   *error;
{
   int    u;

   ENTRY ("cus_dtext", "");

   *error = NOERR;

   if      (which == CUS_GREET)  u = XCGR;
   else if (which == CUS_INTRO)  u = XCIN;
   else    { *error = DBADACT;   RETURN (0); }

   if (NOT unit_lock (u, WRITE, L(cnum), L(0), L(0), L(0), nullchix))
                              *error = DNOCONF;
   else {
      if (NOT unit_make (u))  *error = DNOCONF;
      else {
         unit_write (u, ctext);
         unit_close (u);
      }
      unit_unlk (u);
   }

   RETURN (*error == NOERR);
}
