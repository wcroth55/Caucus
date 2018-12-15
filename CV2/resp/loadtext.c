/*** LOADTEXT.  Load a text partfile into memory.
/
/    ok = loadtext (u, cnum, item, resp, lnum, tbuf);
/
/    Parameters:
/       int     ok;      (success?)
/       int     u;       (unit number of partfile)
/       int     cnum;    (conference number)
/       int     item;    (item number of partfile)
/       int     resp;    (etc)
/       int4    lnum;    (etc)
/       Textbuf tbuf;    (textbuffer to load text into)
/
/    Purpose:
/       Load a particular text partfile (already locked by the caller on
/       unit U) into text buffer tbuf.  Set up all the information in it
/       so that getline() can pluck out what it needs.
/
/    How it works:
/
/    Returns: 1 on success, 0 on error.
/
/    Error Conditions:
/       Can't view file locked on unit U.
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
/    Home:  resp/loadtext.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CR  6/20/91 21:12 Add comments, handle orighost/origresp info. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CX 10/15/91 16:18 Textbuf and Chix-ify loadtext(). */
/*: JV 12/16/91 17:10 Change LNUM to int4. */
/*: JX  5/15/92 17:14 Clean up. */
/*: JX  6/19/92 12:05 Set tbuf->r_last. */
/*: CR 12/07/92 13:36 Add CNUM argument. */
/*: CL 12/11/92 11:18 Long Chxcatsub args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR 12/15/92 17:59 Ensure r_last >= r_first. */
/*: CR  1/22/93 13:58 Initialize 1st response in tbuf. */
/*: CK  3/18/93 18:05 Original resp# is *3rd* token in "+1" hdr. */
/*: JV  3/11/93 10:16 Add File Attachment header line. */
/*: JV  6/06/93 11:33 Add text length info. */
/*: CP  8/17/93 13:45 May have changed something, not sure. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"

extern long memused;

FUNCTION   loadtext (u, cnum, item, resp, lnum, tbuf)
   int     u, item, resp;
   int4    lnum;
   Textbuf tbuf;
{
   short   b;
   int     strstart, this, success;
   int4    c0, c1, rnum, temp;
   Chix    str, rest, format, size, lentmp;
   long    localused;

   ENTRY ("loadtext", "");

   localused = memused;

   tbuf->type      = u;
   tbuf->r_used    = 0;
   tbuf->l_used    = 0;
   tbuf->item      = item;
   tbuf->r_first   = resp;
   tbuf->r_last    = resp - 1;
   tbuf->firstline = lnum;
   tbuf->lastline  = lnum - 1;
   tbuf->cnum      = cnum;

   /*** Initialize the first response in tbuf, just in case this
   /    is partfile 2...N of a single (large!) N-partfile response. */
   b = 0;
   tbuf->r_start[0] =  0;
   tbuf->r_stop [0] = -1;

   if (NOT unit_view (u))  RETURN (0);

   str  = chxalloc (L(100), THIN, "loadtext str");
   rest = chxalloc (L(100), THIN, "loadtext rest");
   size   = chxalloc (L(16), THIN, "loadtext size");
   format = chxalloc (L(16), THIN, "loadtext format");
   lentmp = chxalloc (L(16), THIN, "loadtext lentmp");

   while (unit_read (u, str, L(0))) {
      strstart = 0;
      c0 = chxvalue (str, L(0));

      if (c0 == '#') {
         b = tbuf->r_used++;
         tbuf->r_start[b] = tbuf->l_used;
         tbuf->r_stop [b] = tbuf->l_used - 1;
#if C_LINK
         if (tbuf->orighost[b] == nullchix)
             tbuf->orighost[b] =  chxalloc (L(20), THIN, "loadtext orighost[]");
         chxclear (tbuf->orighost[b]);
         tbuf->origresp[b] = -1;
#endif
         clear_attach (tbuf->attach[b]);
         tbuf->textprop[b] =  0;
         tbuf->bytes[b]    = tbuf->lines[b] = 0;
         tbuf->r_last++;
         tbuf->lastline = -1;
         chxclear (tbuf->copied[b]);
      }

      else if (c0 == '+') {
         c1 = chxvalue (str, L(1));
         switch (c1) {
         case '1':
#if C_LINK
           chxtoken (tbuf->orighost[b], nullchix, 2, str);
           chxtoken (rest,              nullchix, 3, str);
           if (NOT chxnum (rest, &rnum))  rnum = -1;
           tbuf->origresp[b] = rnum;
#endif
           break;

         case '2':   /* Parse and store info for attachment header line. */
           if (tbuf->attach[b] == (Attachment) NULL)
              tbuf->attach[b] = make_attach();
           par_att_header (tbuf->attach[b], str);
           break;

        case '3':    /* Length information. */
           chxtoken (lentmp, nullchix, 2, str);
           chxnum   (lentmp, &(tbuf->lines[b]));
           chxtoken (lentmp, nullchix, 3, str);
           chxnum   (lentmp, &(tbuf->bytes[b]));
           break;

        case '4':
           chxtoken (lentmp, nullchix, 2, str);
           chxnum   (lentmp,  &temp);
           tbuf->textprop[b] = temp;
           break;

        case '5':
           chxtoken (nullchix, tbuf->copied[b], 2, str);
           break;

        default:
           break;
        }
        continue;  /* Don't write this line to the tbuf! */
      }
      else if (c0 == ' ')  strstart = 1;

      this = tbuf->l_used++;
      chxclear  (tbuf->line[this]);
      chxcatsub (tbuf->line[this], str, L(strstart), ALLCHARS);

      ++tbuf->r_stop[b];
      ++tbuf->lastline;
   }
   if (tbuf->r_last < tbuf->r_first)  tbuf->r_last = tbuf->r_first;

   unit_close(u);

   SUCCEED;

done:
   chxfree (str);
   chxfree (rest);
   chxfree (size);
   chxfree (format);
   chxfree (lentmp);

/* if (memused > localused) {
      fprintf (stderr, "%9d %9d loadtext(%d %d %d)\n", 
               memused, memused - localused, cnum, item, resp);
   }
*/

   RETURN (success);
}
