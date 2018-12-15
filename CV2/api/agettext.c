/*** A_GET_TEXT.   Load a Atext object from a file unit number.
/
/    ok = a_get_text (text, n);
/
/    Parameters:
/       int    ok;      (return code)
/       Atext  text;    (object to be filled)
/       int    n;       (unit number)
/
/    Purpose:
/       Load a text object.
/
/    How it works:
/       Unit N must already be locked by the caller.  A_get_text() reads
/       the entire contents of the file into TEXT.
/
/    Returns: A_OK    on success
/             A_DBERR on error
/
/    Error Conditions: can't read from unit N.
/  
/    Side effects: 
/
/    Related functions:
/
/    Called by:  a_get...() functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/agettext.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/17/92 22:49 New function. */
/*: CR 11/30/92 17:20 Replace Text with Atext. */
/*: CR 12/07/92 10:46 Add unit_read() 3rd arg. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  a_get_text (Atext text, int n)
{
   Chix   line, newl;

   chxclear (text->data);
   text->lines = 0;

   if (NOT unit_view (n))  return (A_DBERR);

   line = chxalloc (L(100), THIN, "a_get_te line");
   newl = chxalloc ( L(10), THIN, "a_get_te newl");
   chxcpy (newl, CQ("\n"));

   while (unit_read (n, line, L(0))) {
      chxcat (text->data, line);
      chxcat (text->data, newl);
      text->lines++;
   }

   unit_close (n);

   chxfree (line);
   chxfree (newl);
   return  (A_OK);
}
