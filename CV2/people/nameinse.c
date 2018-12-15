
/*** NAMEINSERT.   Insert NAME into a name partfile list in a text buffer.
/
/    nameinsert (name, slot, tbuf);
/
/    Parameters:
/       Chix    name;        (name to be inserted)
/       int     slot;        (at which slot in the partfile list)
/       Textbuf tbuf;        (in this text buffer)
/
/    Purpose:
/       Textbuf TBUF contains a list of names, taken from a name partfile.
/       Insert string NAME into that list, at position SLOT.
/
/    How it works:
/       The names starting at SLOT through the end of the list are
/       moved down one, and NAME inserted at SLOT.
/
/    Returns: nothing
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  modnames(), ??
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/nameinse.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: JX  5/15/92 16:59 PFILEBUF->Textbuf, chixify. */
/*: CX  5/27/92 12:27 Guruized. */

#include <stdio.h>
#include "caucus.h"

FUNCTION   nameinsert (name, slot, tbuf)
   Chix    name;
   int     slot;
   Textbuf tbuf;
{
   int     last, i;

   /*** LAST will become our new last slot.  Free anything there. */
   last = tbuf->l_used++;
   if (tbuf->line[last] != nullchix)  chxfree (tbuf->line[last]);

   /*** Shift everything after SLOT "down" one. */
   for (i=last;   i>slot;   --i)  
      tbuf->line[i] = tbuf->line[i-1];

   /*** Allocate a new chix at SLOT, and copy in NAME. */
   tbuf->line[slot] = chxalloc (L(60), THIN, "nameinsert tbuf->line");
   chxcpy (tbuf->line[slot], name);
}
