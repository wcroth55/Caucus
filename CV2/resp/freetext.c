
/*** FREE_TEXTBUF.   Deallocate a textbuffer.
/
/    free_textbuf (t);
/
/    Parameters:
/       Textbuf t;       ( Text buffer.)
/
/    Purpose:
/       Deallocate a text buffer created by make_textbuf().
/
/    How it works:
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  resp/freetext.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/15/91 14:17 New function. */
/*: JX  6/16/92 09:17 Don't free chix if they weren't allocated. */
/*: JV  6/06/93 11:59 Free lines & bytes. */
/*: CP  8/01/93 14:29 Free attachments! */
/*: CP  8/01/93 16:14 Add mem_debug() call. */
/*: CK  8/02/93 12:48 Free_attach requires address of arg. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "chixuse.h"
#include "handicap.h"
#include "textbuf.h"

extern int debug;

FUNCTION   free_textbuf (t)
   Textbuf t;
{
   int     i;

   ENTRY ("free_textbuf", "");

   if (t == nulltext  ||  t->type == TB_IS_UNALLOC)  RETURN (1);

   if (debug)  mem_debug (MEM_FREED, t->dname, "tbuf", L(999));
   sysfree ( (char *) t->r_start);
   sysfree ( (char *) t->r_stop);
#if C_LINK
   sysfree ( (char *) t->origresp);
#endif
   sysfree ( (char *) t->lines);
   sysfree ( (char *) t->bytes);
   sysfree ( (char *) t->textprop);

#if C_LINK
   for (i=0;   i<t->r_max;   ++i) 
      if (t->orighost[i] != nullchix) chxfree (t->orighost[i]);
   sysfree ( (char *) t->orighost);
#endif

   for (i=0;   i<t->l_max;   ++i) 
      if (t->line[i] != nullchix) chxfree (t->line[i]);
   sysfree ( (char *) t->line);

   for (i=0;   i<t->r_max;   ++i) {
      if (t->attach[i] != (Attachment) NULL)  free_attach (&(t->attach[i]));
      chxfree (t->copied[i]);
   }
   sysfree ( (char *) t->attach);
   sysfree ( (char *) t->copied);

   t->type = TB_IS_UNALLOC;
   sysfree ( (char *) t);

   RETURN (1);
}
