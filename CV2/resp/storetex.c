/*** STORETEXT.  Store (part of) a text file from memory to the actual file.
/
/    ok = storetext (uf, r0, r1, tbuf);
/
/    Parameters:
/       int     ok;      (success?)
/       int     uf;      (partfile unit code number)
/       int     r0;      (first "response" or chunk)
/       int     r1;      (last  "response" or chunk)
/       Textbuf tbuf;    (textbuffer to write from)
/
/    Purpose:
/       Writes responses (chunks) R0 through R1 of the textbuffer tbuf
/       out to file UF.  This file must already have been locked by the caller.
/
/    How it works:
/
/    Returns: 1 on success, 0 on error.
/
/    Error Conditions:
/       Can't make file locked on unit UF.
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
/    Home:  resp/storetex.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:52 Source code master starting point */
/*: CR  6/24/91 15:14 Write orighost/origresp info; new header comments. */
/*: CX 10/15/91 18:02 Textbuf-and-Chix -ify storetext(). */
/*: CK  3/18/93 18:48 Only write "+1" if real header starts with "#". */
/*: JV  3/01/93 09:43 Add File Attachments. */
/*: JV  6/06/93 11:50 Add text length info. */
/*: CP  8/16/93 14:18 +2 gets db_name, not real name, of attachment file. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"

FUNCTION   storetext (uf, r0, r1, tbuf)
   int     uf, r0, r1;
   Textbuf tbuf;
{
   short   r, m;
   int4    c1;
   char    tstr[40];
   Chix    temp, temp2, header;

   ENTRY ("storetext", "");

   if (NOT unit_make (uf))  RETURN (0);

   temp  = chxalloc (L(80), THIN, "storetext temp");
   temp2 = chxalloc (L(80), THIN, "storetext temp2");

   /*** For each response in the list... */
   for (r=r0;   r<=r1;   ++r) {

      /*** Write the "#" header line. */
      header = tbuf->line [ tbuf->r_start[r] ];
      unit_write (uf, header);
      unit_write (uf, NEWLINE);

      /*** Write the "+1" original response host & number line. */
      c1 = chxvalue (header, L(0));
#if C_LINK
      if (c1=='#'  &&  tbuf->origresp[r] > 0) {
         chxclear  (temp);
         chxformat (temp, CQ("+1 %s %d\n"), (int4) tbuf->origresp[r], L(0), 
                                                   tbuf->orighost[r], nullchix);
         unit_write (uf, temp);
      }
#endif

      /*** Write the "+2" attachment header line. */
      if ((tbuf->attach[r] != (Attachment) NULL) &&
          (tbuf->attach[r]->size > 0)) {
         chxclear  (temp);
         chxformat (temp, CQ("+2 %s %d %s %d\n"),
                    tbuf->attach[r]->size, L(tbuf->attach[r]->rec_len),
                    tbuf->attach[r]->format->format, tbuf->attach[r]->db_name);
         unit_write (uf, temp);
      }

      /*** Write the "+3" text length header line. */
      if (tbuf->lines[r] > 0) {
         chxclear  (temp);
         chxformat (temp, CQ("+3 %d %d\n"), tbuf->lines[r], tbuf->bytes[r],
                          nullchix, nullchix);
         unit_write (uf, temp);
      }

      if (tbuf->textprop[r] > 0) {
         sprintf (tstr, "+4 %d\n", tbuf->textprop[r]);
         unit_write (uf, CQ(tstr));
      }

      /*** Write the "+5" 'this response copied from' information. */
      if (chxvalue (tbuf->copied[r], L(0))) {
         unit_write (uf, CQ("+5 "));
         unit_write (uf, tbuf->copied[r]);
         unit_write (uf, CQ("\n"));
      }

      /*** Write the text lines, indented one space. */
      for (m=tbuf->r_start[r]+1;   m<=tbuf->r_stop[r];   ++m) {
         unit_write (uf, CQ(" "));
         unit_write (uf, tbuf->line[m]);
         unit_write (uf, NEWLINE);
      }
   }
   unit_close (uf);

   chxfree (temp);
   chxfree (temp2);
   RETURN  (1);
}
