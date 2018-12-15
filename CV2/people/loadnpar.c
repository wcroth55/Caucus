/*** LOADNPART.  LOAD Name PARTfile.
/
/     ok = loadnpart (n, p, ppfilebuf);
/
/  Parameters:
/     int       ok;         (success?)
/     int       n;          (unit number of locked partfile)
/     int       p;          (file number, e.g. 002 in title002...)
/     PFILEBUF *ppfilebuf;  (load into this text buffer)
/
/  Purpose:
/    Loads name partfile P, file type N into the textbuffer structures.
/    The caller can either specify a PFILEBUF to use for the 
/    textbuffer structures, or use (PFILEBUF *) NULL to specify the
/    global structures TEXT, LINE, and BLOCK.
/
/    Assumes file is already locked.
/
/  Returns:
/
/  Error Conditions:
/
/  Side Effects:
/     Changes the contents of global data structures TEXT, LINE, and BLOCK
/     unless you specify a PFILEBUF for loadnpart() to use.
/
/  Called by:
/     cmd1/descsubj.c   cmd1/itempers.c    cmd1/showpers.c 
/     cmd1/showsubj.c   cmd2/check.c:      gen/listconf.c: 
/     people/matchnl.c  people/modnames.c  xcheck/allconfs.c 
/     xfp/nextconf.c    xfp/nextconf.c     xfp/nextneig.c 
/     xfp/nextuser.c    xfp/nextuser.c     xmaint/cremove.c 
/
/  Home: people/loadnpar.c
/
/  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: JV  5/15/91 16:15 Add optional PFILEBUF arg and header docs. */
/*: JX  5/15/92 16:25 Convert PFILEBUF to Textbuf. */
/*: CX  5/27/92 12:05 Guruizing... */
/*: DE  6/01/92 17:00 Patched unit_read */
/*: JX  6/15/92 18:09 Actually use mybuf instead of tbuf. */

#include <stdio.h>
#include "caucus.h"

extern Textbuf tbuf;

FUNCTION   loadnpart (n, p, private_buf)
   int     n, p;
   Textbuf private_buf;
{
   Chix     str;
   Textbuf  mybuf;
   int      used;

   ENTRY ("loadnpart", "");

   /*** Use global or private? */
   if (private_buf == (Textbuf) NULL) mybuf = tbuf;
   else                               mybuf = private_buf;

   mybuf->item    = -1;
   mybuf->type    =  n;
   mybuf->r_first =  p;

   if (NOT unit_view (n))  RETURN(0);

   str = chxalloc (L(100), THIN, "loadnpart str");
   for (used=0;   unit_read (n, str, L(0));   ) {
      if (mybuf->line[used] == nullchix)
          mybuf->line[used] =  chxalloc (L(60), THIN, "loadnpart mybuf->line");

      chxcpy (mybuf->line[used++], str);
   }
   chxfree (str);
   mybuf->l_used = used;

   unit_close(n);
   RETURN (1);
}
