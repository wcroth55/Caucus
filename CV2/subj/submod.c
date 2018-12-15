
/*** SUBMOD.   Modify the list of items filed under a subject.
/
/    ok = submod (cnum, utd, utf, owner, subnum, range, op);
/
/    Parameters:
/       int    ok;          (success?)
/       int    cnum;        (conference number)
/       int    utd;         (subject directory:        XCSD or XUSD)
/       int    utf;         (subject text file:        XCSF or XUSF)
/       char  *owner;       (psubject owner id)
/       int    subnum;      (subject number)
/       short *range;       (array of items selected)
/       int    op;          (1=>remove items, 0=>add items)
/       int    n;           (subject number)
/
/    Purpose:
/       Add items to a subject category, or remove items from that subject.
/
/    How it works:
/       Submod() works with the subject category identified by the conference
/       number CNUM, the unitcodes UTD and UTF, the OWNER (if this is
/       a PSUBJECT) userid, and the subject number SUBNUM.
/
/       RANGE is an array of item numbers.  Item number I is in the range
/       if RANGE[I] is >= 0.
/
/       OP==0 means "add    RANGE items to   the subject."
/       OP==1 means "remove RANGE items from the subject."
/
/    Returns: 1 on success, 0 on database error.
/
/    Error Conditions:
/ 
/    Side effects: May reload global "master".
/
/    Related functions:
/
/    Called by:  chg_dsubject() only!
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  subj/submod.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:56 Source code master starting point */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  4/09/91 13:51 Add new confnum argument to updatetext() call. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: DE  3/24/92 12:12 Chixified */
/*: DE  4/20/92 12:40 Added arg to getline2 */
/*: CX  5/28/92 18:02 Add tbuf arg to updatepart() call. */
/*: CR 10/11/92 22:13 Remove CQ from parsenum(). */
/*: JV  5/04/93 10:00 Add attach arg to updatepart() call. */
/*: CN  5/15/93 17:11 Use new item selection codes. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"

extern struct master_template master;
extern union  null_t          null;
extern Textbuf                tbuf;

FUNCTION  submod (cnum, utd, utf, owner, sub, range, op)
   int    utd, utf, sub, op;
   short  range[];
   Chix   owner;
{
   Chix   header, title, empty;
   int    success, pass;
   Chix   s;
   short  i;
   short *select;
   Chix   subtext;
   Chix   getline2();
   char  *sysmem();

   ENTRY ("submod", "");

   header  = chxalloc (L(100), THIN, "submod header");
   title   = chxalloc (L(100), THIN, "submod title");
   empty   = chxalloc (  L(4), THIN, "submod empty");
   subtext = chxalloc (L(200), THIN, "submod subtext");
   success = 0;

   /*** Reload the "master" struct if wrong conf. */
   if (master.cnum != cnum  &&  NOT srvldmas (1, &master, cnum))         FAIL;

   /*** Save the Header and Title of SUB. */
   if ((s = getline2 (utd, utf, cnum, owner, 0, sub, 0, (Textbuf) NULL))
                                                            == null.chx) FAIL;
   chxcpy (header, s);
   chxcpy (title,  getline2 (utd, utf, cnum, owner, 0, sub, 1, (Textbuf) NULL));

   /*** Prepare the SELECT buffer for the list of items in this subject. */
   select = (short *) sysmem (L(sizeof(short)) * L(master.items+3), "subjselect");
   if  (select == (short *) NULL)                                 FAIL;
   for (i=0;   i<=master.items;   ++i)  select[i] = -1;

   /*** Load the list of items in SUB into SELECT. */
   for (i=2;
       (s = getline2 (utd, utf, cnum, owner, 0, sub, i, (Textbuf) NULL))
           != null.chx;
       ++i) parsenum (s, select, master.items, 0);

   /*** Add or remove RANGE to SELECT, according to OP. */
   for (i=1;   i<=master.items;   ++i) if (range[i] >= 0) {
      if (op)  select[i] = -1;
      else     select[i] =  0;
   }

   /*** Print the result into the XITX file, then update the text in
   /    the subject file with the contents of XITX. */
   pass = 1;
   unit_lock  (XITX, WRITE, L(0), L(0), L(0), L(0), owner);
   unit_make  (XITX);
   printrange (XITX, select, master.items, 0);
   unit_close (XITX);
   sysfree    ( (char *) select);

   if (chixfile (XITX, subtext, ""))
      updatepart (utf, cnum, 0, sub, owner, subtext, header, title, -1,
                   nullchix, (Attachment) NULL, tbuf, 0, nullchix);
   else pass = 0;

   unit_kill  (XITX);
   unit_unlk  (XITX);

   DONE(pass);

done:
   chxfree (header);
   chxfree (title);
   chxfree (empty);
   chxfree (subtext);
   RETURN  (success);
}
