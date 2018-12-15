/*** UPDATEPART.    Update a response in a text partfile.
/
/    updatepart (uf, cnum, item, resp, owner, ctext, newheader, newtitle, 
/                 oresp, ohost, attach, tbuf, prop, copied);
/
/    Parameters:
/       int     uf;          (partfile unit code number)
/       int     cnum;        (conference number)
/       int     item;        (item number, 0 for messages/etc.)
/       int     resp;        (response or message number)
/       Chix    owner;       ("" for items, userid of owner for messages)
/       Chix    ctext;       (text of response/message/etc.)
/       Chix    newheader;   (new header field)
/       Chix    newtitle;    (new title field, items only)
/       int     oresp;       (original response number, if from another host)
/       Chix    ohost;       (original host)
/       Attachment attach;   (info about attachment)
/       Textbuf tbuf;        (do temporary work in this text buffer.)
/       int     prop;        (text property)
/       Chix    copied;      (response copied from? info)
/
/    Purpose:
/       Updatepart() does most of the real work in adding or updating
/       text in a text partfile.  This includes item/responses, messages,
/       osubjects, and psubjects.  Given a chunk of text in the database
/       (such as a response or message) completely specified by UF, CNUM, 
/       ITEM, RESP, and OWNER, updatepart() replaces that chunk with the
/       contents of CTEXT, NEWHEADER, and NEWTITLE.
/
/       If ORESP > 0 and OHOST is not empty (or null), this response (chunk)
/       was originally from another host.  Mark it with the name of the 
/       original host (OHOST) and its original response number on that 
/       host (ORESP).
/
/       Updatepart() takes care of all the splitting/creating/etc. of text
/       partfiles needed in order to make CTEXT fit.
/
/    How it works:
/       The corresponding "directory" unit code number for UF must already
/       be locked by the caller.
/
/       Note that the new, completely chixified version of updatepart()
/       now limits the size of partfiles by # of lines *only*, and
/       doesn't really care about the number of characters.
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/updatepa.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/23/91 15:38 New function, based on old updatetext(). */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR  8/07/91 14:25 Add 'confnum' arg to syspdir() call. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JV 12/16/91 16:34 Change pdir.lnum -> int4. */
/*: CR  1/03/92 22:19 Indent 1st line of 2nd & subsequent partfiles. */
/*: CR  1/20/92 16:41 Add cnum arg to frozen(). */
/*: CX  5/27/92 15:38 Chixified. */
/*: CX  6/29/92 14:16 Don't null tbuf chix lines, swap them! */
/*: CR 10/27/92 17:08 Add RNUM to correct for resp # vs index # bug. */
/*: CR 10/27/92 17:58 Check both ORESP & OHOST for imported response. */
/*: CR 10/29/92 14:22 use r_used - 1 ! */
/*: CR 12/07/92 13:38 Add CNUM arg to loadtext(). */
/*: CR 12/11/92 13:14 Chxnextl last arg is int4 *. */
/*: JV  2/27/93 23:30 Add Attach arg & second header extension. */
/*: JV  5/25/93 17:29 Add code to handle attachments. */
/*: JV  6/05/93 22:44 Add code to calc bytes of text. */
/*: CR 10/18/93 12:56 "FITS" computation must be most restrictive one.*/
/*: CR  3/14/95 18:18 Do not write XUMF partfiles that are empty! */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"

#define  SWAPCHX(a,b) { Chix c;  c=a;  a=b;  b=c; }

extern struct pdir_t         pdir;
extern union  null_t         null;

FUNCTION  updatepart (uf, cnum, item, resp, owner, ctext, newheader, 
                      newtitle, oresp, ohost, attach, tbuf, prop, copied)
   int     uf, cnum, item, resp, oresp, prop;
   Chix    newheader, newtitle, owner, ohost, ctext, copied;
   Textbuf tbuf;
   Attachment attach;
{
   int    needslines, new_start, rnum, from_another;
   int    p, q, r, space, fits, lnum, l_used, suggest_max;
   int4   linepos;
   Chix   templine;

   ENTRY ("updatepart", "");

   /*** Was this response originally from another host? */
   from_another = (oresp > 0  &&  ohost!=nullchix  &&  NOT EMPTYCHX(ohost));

   /*** The suggested max number of lines for a partfile is half
   /    of the real maximum.  This keeps down the average text
   /    partfile size, now that we are only using number of lines
   /    to limit partfile size. */
   suggest_max = tbuf->l_max / 2;

   /*** Lock/W and load the partfile directory for this ITEM. */
   syspdir   (&pdir, uf, cnum, item, owner);

   /*** Find which partfile (index P) contains this response. */
   p = partnum (&pdir, resp, 0);

   /*** If RESP is a new response, added on to the very end of this
   /    ITEM, look at the last response partfile.  If it is one part
   /    of a multi-partfile response, then RESP must begin its very
   /    own new partfile. */
   if (p == pdir.parts-1  &&  pdir.lnum[p] > 0) {
      pdir.resp[++p] = resp;
      pdir.lnum[  p] = L(0);
      pdir.resp[++pdir.parts] = 16000;
   }


   /*** If this response covers multiple partfiles P through Q,
   /    delete partfiles P+1 through Q. */
   for (q=p+1;   q<pdir.parts  &&  pdir.resp[q] == pdir.resp[p];   ++q) {
      unit_lock (uf, WRITE, L(cnum), L(item), L(pdir.resp[q]), pdir.lnum[q],
                     owner);
      unit_kill (uf);
      unit_unlk (uf);
   }

   /*** Lock/W & load the partfile containing this response.  (Multi-partfile
   /    responses have already had all but their first part deleted.) */
   unit_lock (uf, WRITE, L(cnum), L(item), L(pdir.resp[p]), pdir.lnum[p],
                  owner);
   loadtext  (uf, cnum, item, pdir.resp[p], pdir.lnum[p], tbuf);


   /*** From this point, there are 4 possible cases:
   /
   /     (a) RESP is NOT the last response in the buffer, and the new
   /         CTEXT will fit into the free space in the buffer.  Load it
   /         in, write out the buffer, and we're done.
   /
   /     (b) RESP is NOT the last response in the buffer, but the new
   /         CTEXT is too big to fit into the free space in the buffer.
   /         Write the responses *after* RESP into their own partfile,
   /         remove them from the buffer, and go to case (D).
   /
   /     (c) RESP is a completely new response.  Create the entries for
   /         an empty response in its place, and go to case (D).
   /
   /     (d) RESP is the last response in the buffer.  Delete it and recover
   /         the space.  If the new CTEXT fits in the available space, load
   /         it in and write out the buffer.  Otherwise, write everything 
   /         before RESP out as one partfile.  Then write out the new CTEXT
   /         as one (or more) partfiles.
   /
   /     Note that a response may be the "last" (undeleted) one in a buffer,
   /     yet still fall into category (a) or (b), because of deleted responses
   /     after it that have an empty entry. */

   /*** Count up # lines & chars needed by text in CTEXT, plus size of
   /    header and title. */
   templine = chxalloc (L(160), THIN, "updatepart templine");
   for (needslines=linepos=0;   chxnextline (templine, ctext, &linepos);
      ++needslines);

   /*** Store them for later. */
   tbuf->lines[resp-tbuf->r_first] = needslines;
   tbuf->bytes[resp-tbuf->r_first] = linepos;

   /*** To help determine if this is case (A) or (B), conservatively 
   /    estimate the amount of SPACE freed by deleting RESP, and see if
   /    the new CTEXT FITS. */
   if (resp < tbuf->r_last) {
      r     = resp - tbuf->r_first;
      space = tbuf->r_stop[r] - tbuf->r_start[r] + 1;
      fits  = needslines < suggest_max - tbuf->l_used + space - 10;
   }

   /*** Case (A): CTEXT will fit into the text buffer. */
   if (resp < tbuf->r_last  &&  fits) {

      /*** If we can, recover the SPACE freed by deleting RESP by
      /    shifting the text of the remaining responses up. */
      if (space > 0) {

         /*** Shift the lines of the remaining responses upward to fill
         /    the freed space. */
         r      = resp - tbuf->r_first;
         l_used = tbuf->r_start[r];
         for (rnum=resp+1;   rnum<=tbuf->r_last;   ++rnum) {
            new_start = l_used;
            r = rnum - tbuf->r_first;
            for (q=tbuf->r_start[r];   q<=tbuf->r_stop[r];   ++q) {
               SWAPCHX  (tbuf->line[l_used], tbuf->line[q]);
               chxclear (tbuf->line[q]);
               ++l_used;
            }
            tbuf->r_start[r] = new_start;
            tbuf->r_stop [r] = l_used - 1;
         }
         tbuf->l_used = l_used;
      }

      /*** Put the new CTEXT after all the rest of the responses. */
      r = resp - tbuf->r_first;
      tbuf->r_start [r] = tbuf->l_used;
      tbuf->textprop[r] = prop;
      if (copied != nullchix)  chxcpy (tbuf->copied[r], copied);
#if C_LINK
      if (from_another) {
         tbuf->origresp[r] = oresp;
         if (tbuf->orighost[r] == nullchix)
             tbuf->orighost[r] =  chxalloc (L(20), THIN, "updatepart orighost");
         chxcpy (tbuf->orighost[r], ohost);
      }
      else tbuf->origresp[r] = -1;
#endif

      /*** Put any attachment info into the tbuf. */
      if (attach != (Attachment) NULL && attach->size > 0) {
         if (tbuf->attach[r] == (Attachment) NULL) 
            tbuf->attach[r] = make_attach();
         copy_attach (tbuf->attach[r], attach);
      } else 
         if (attach == (Attachment) NULL)
            free_attach (&(tbuf->attach[r]));
         else if (attach->size == 0)
            free_attach (&(tbuf->attach[r]));

      addline (r, newheader, tbuf);
      if (newtitle != nullchix)  addline (r, newtitle, tbuf);

      for (linepos=0;   chxnextline (templine, ctext, &linepos);   ) {
         chxtrim  (templine);
         addline  (r, templine, tbuf);
      }

      storetext (uf, 0, tbuf->r_used-1, tbuf);
      unit_unlk (uf);
      tbuf->item = -1;
      chxfree (templine);
      RETURN  (1);
   }


   /*** Case (B): CTEXT will not fit into the text buffer.  Make everything
   /    after RESP into a new partfile. */
   if (resp < tbuf->r_last  &&  NOT fits) {
      unit_unlk (uf);

      /*** Write new partfile (but don't write empty message partfiles!) */
      if (uf != XUMF  ||  NOT is_empty (tbuf, r+1, tbuf->r_used-1)) {
         unit_lock (uf, WRITE, L(cnum), L(item), L(tbuf->r_first + r + 1), L(0),
                        owner);
         storetext (uf, r+1, tbuf->r_used-1, tbuf);
         unit_unlk (uf);
      }
      unit_lock (uf, WRITE, L(cnum), L(item), L(pdir.resp[p]), pdir.lnum[p],
                     owner);

      tbuf->r_last = tbuf->r_first + r;
   }


   /*** In case (C), just make an empty response RESP in the text buffer. */
   if (resp > tbuf->r_last) {
      tbuf->r_last = resp;
      r = resp - tbuf->r_first;
      tbuf->r_start[r] = tbuf->l_used;
      tbuf->r_stop [r] = tbuf->l_used - 1;
   }


   /*** From here on, all cases have turned into case (D): RESP is the
   /    last response in the buffer. */

   /*** Start by removing RESP, and resetting the free space markers
   /    to re-use everything starting at the old location of RESP.   
   /    Figure if the new CTEXT FITS into the remaining text buffer space. */
   r         = resp - tbuf->r_first;
   tbuf->l_used = tbuf->r_start[r];
   space = tbuf->r_stop[r] - tbuf->r_start[r] + 1;
   fits  = needslines < suggest_max - tbuf->l_used + space - 10;

   /*** If the new CTEXT does not fit, write out everything before it
   /    as a partfile, and clear the text buffer. */
   if (NOT fits  &&  r>0 ) {
      if (uf != XUMF  ||  NOT is_empty (tbuf, 0, r-1))
         storetext (uf, 0, r-1, tbuf);
      unit_unlk (uf);
      unit_lock (uf, WRITE, L(cnum), L(item), L(resp), L(0), owner);
      tbuf->l_used = 0;
      r = 0;
   }

   /*** Now proceed to add CTEXT to the text buffer.  If it overflows the
   /    text buffer, we'll just write additional partfiles as needed. */
   tbuf->r_start [r] = tbuf->l_used;
   tbuf->textprop[r] = prop;
   if (copied != nullchix)  chxcpy (tbuf->copied[r], copied);
#if C_LINK
   if (from_another) {
      tbuf->origresp[r] = oresp;
      if (tbuf->orighost[r] == nullchix)
          tbuf->orighost[r] =  chxalloc (L(20), THIN, "updatepart orighost");
      chxcpy (tbuf->orighost[r], ohost);
   }
   else tbuf->origresp[r] = -1;
#endif

   addline (r, newheader, tbuf);

   if (attach != (Attachment) NULL && attach->size > 0) {
      if (tbuf->attach[r] == (Attachment) NULL)
         tbuf->attach[r] = make_attach();
      copy_attach (tbuf->attach[r], attach);
   } else 
      if (attach == (Attachment) NULL)
         free_attach (&(tbuf->attach[r]));
      else if (attach->size == 0)
         free_attach (&(tbuf->attach[r]));

   lnum = 1;
   if (newtitle != null.chx) { addline (r, newtitle, tbuf);   ++lnum; }

   /*** Pull lines one at a time from CTEXT, and add them to the text
   /    buffer.  If the buffer gets full, write it out as multiple
   /    partfiles.
   /
   /    Note that the "too full" computation here is more generous (allows 
   /    bigger responses in one partfile) than the computation
   /    used in the "fits" computation, above.  This is required, else
   /    partfiles may get split in funky ways. */
   for (linepos=0;   chxnextline (templine, ctext, &linepos);   ++lnum) {
      chxtrim (templine);

      if (tbuf->l_used + 1 > suggest_max - 5) {
         storetext (uf, 0, r, tbuf);
         unit_unlk (uf);

         unit_lock (uf, WRITE, L(cnum), L(item), L(resp), L(lnum), owner);
         tbuf->l_used = r = 0;
         tbuf->r_start[r] = 0;

         /*** Because storetext() thinks the 1st line of a block is always
         /    a header line, indent TEMPLINE 1 char to disabuse storetext(). */
         chxconc (templine, CQ(" "), templine);
      }

      addline (r, templine, tbuf);
   }

   storetext (uf, 0, r, tbuf);
   unit_unlk (uf);
   tbuf->item = -1;

   /*** If the response partfile directory is nearly full (and not
   /    already frozen), then declare this item frozen. */
   if (pdir.parts >= PDIRMAX  &&  NOT frozen (cnum, item, 0))
      itemfreeze (cnum, item, 2);
   chxfree (templine);
   RETURN  (1);
}



FUNCTION   addline (r, str, tbuf)
   int     r;
   Chix    str;
   Textbuf tbuf;
{
   int     m;

   tbuf->r_stop[r] = m = tbuf->l_used++;
   if (tbuf->line[m] == nullchix)
       tbuf->line[m] =  chxalloc (L(60), THIN, "addline line[m]");
   chxcpy (tbuf->line[m], str);
}
