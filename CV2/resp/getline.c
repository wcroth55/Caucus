/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** GETLINE.   Get a line of conference text.
/
/    line = getline2 (ud, uf, cnum, owner, item, response, linenum, tbuf);
/
/    Parameters:
/       Chix    line;      (line of text returned in chix.  Don't chixfree!)
/       int     ud;        (unitcode number of directory file)
/       int     uf;        (unitcode number of text file)
/       short   cnum;      (guess)
/       Chix    owner;     (if PSUBJECT, userid of owner; else nullchix)
/       int     item;      (item number, or ?)
/       int     response;  (response number, or subject number?)
/       int     linenum;   (line number)
/       Textbuf tbuf;      (text buffer to use)
/
/    Purpose:
/       Gets a line of conference text.  The line may be from:
/          an item or response partfile (ud=XCRD, uf=XCRF);
/          an OSUBJECT (ud=XCSD, uf=XCSF) or PSUBJECT (ud=XUSD, uf=XUSF)
/             textfile 
/
/    How it works:
/       The particular text file is selected by UD, UF, and OWNER.
/       The particular line is selected by ITEM, RESPONSE, and LINENUM.
/
/       Getline() looks in the the supplied textbuffer "tbuf" to see if
/       it already contains the appropriate partfile.  If not, getmline()
/       loads the partfile into tbuf via loadtext().
/
/       If the selected line does not exist, getline() returns nullchix.
/
/    Returns: a chix containing the requested line, or
/             nullchix (line does not exist)
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
/    Home:  resp/getline.c
/ */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CR 11/12/88 19:57 Change XCRD -> ud in unlk after failed syspdir. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  8/07/91 14:25 Add 'confnum' arg to syspdir() call. */
/*: CX 10/15/91 15:41 Chixify, update header comments. */
/*: CR 10/21/91 23:43 Add 'cnum' argument. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: JV 12/16/91 16:41 Change pdir.lnum to int4. */
/*: JX  5/13/92 16:37 Actually *define* the arg CNUM (& add to header). */
/*: JX  6/18/92 10:02 Allow use of global TBUF. */
/*: CR 12/07/92 13:32 Check mybuf for proper conference number. */
/*: CR 12/15/92 17:40 Break out failure returns for debugging. */

#include <stdio.h>
#include "caucus.h"

extern struct pdir_t  pdir;
extern union  null_t  null;
extern Textbuf        tbuf;

FUNCTION   Chix getline2 (ud, uf, cnum, owner, item, response, linenum, mybuf)
   int     ud, uf, item, response, linenum;
   short   cnum;
   Chix    owner;
   Textbuf mybuf;
{
   short  right_item, after_first, beyond_last, p, b, m, ok;
   int    success;

   ENTRY ("getline", "");

   if (mybuf == (Textbuf) NULL) mybuf = tbuf;

   /*** Is the desired response attached to the right item (the one we
   /    have loaded in the text buffer)?  Is it at or after the first
   /    response in the buffer?  Is it beyond the last response in the
   /    buffer? */
   right_item  = (item == mybuf->item  &&  mybuf->type == uf  &&
                  cnum == mybuf->cnum);

   after_first = (response >  mybuf->r_first) ||
                 (response == mybuf->r_first  &&  linenum >= mybuf->firstline);

   beyond_last = (response >  mybuf->r_last)  ||
                 (response == mybuf->r_last   &&  linenum >  mybuf->lastline);

   /*** If the desired response & line is outside the current text buffer
   /    structure, figure out what partfile the response is in (if any),
   /    and load it. */
   if (NOT right_item  ||  NOT after_first  ||  beyond_last) {

      /*** If the response/linenum appears to be beyond this partfile, and
      /    yet is not in the next one, then it doesn't exist. */
      if (right_item  &&  after_first  &&  beyond_last  &&  pdir.type==uf) {
         p = partnum (&pdir, response, linenum);
         if (pdir.resp[p] == mybuf->r_first  &&
             pdir.lnum[p] == mybuf->firstline)      DONE (-1);
      }

      /*** Lock the part file directory, and load it. */
      unit_lock (ud, READ,  L(cnum), L(item), L(0), L(0), owner);
      syspdir   (&pdir, uf, cnum, item, owner);
      if (pdir.parts == 0)  { unit_unlk (ud);   DONE (-2); }
      p = partnum (&pdir, response, linenum);

      /*** Lock the desired part file.  (Once we've got it locked, no one
      /    else can change it, so we can let go the part file dir.) */
      ok = unit_lock (uf, READ, L(cnum), L(item), L(pdir.resp[p]), 
                                            pdir.lnum[p], owner);
      unit_unlk (ud);
      if (NOT ok)  DONE (-3);
      ok = loadtext  (uf, cnum, item, pdir.resp[p], pdir.lnum[p], mybuf);
      unit_unlk (uf);
      if (NOT ok)  DONE (-4);
   }

   /*** B is the index (into mybuf->r_start, etc.) of the desired response
   /    in this texmybuffer. */
   b =  response -  mybuf->r_first;
   if (b < 0  ||  response > mybuf->r_last)  DONE (-5);

   /*** M is the linenumber in that response.   P is the index (into 
   /    mybuf->line) of that line. */
   m = (response == mybuf->r_first ? linenum - mybuf->firstline : linenum);
   p = mybuf->r_start[b] + m;
   if (p > mybuf->r_stop[b])  DONE (-6);

   RETURN (mybuf->line[p]);

done:
   /* value of success set here for debugging only. */
   RETURN (null.chx);
}
