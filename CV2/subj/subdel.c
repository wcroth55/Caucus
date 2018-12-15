
/*** SUBDELETE.  Delete a subject category.
/
/    ok = subdelete (cnum, cname, uad,uaf, utd,utf, usd,usf, owner, n);
/
/    Parameters:
/       int    ok;          (success?)
/       int    cnum;        (conference number)
/       char  *cname;       (conference name)
/       int    uad;         (sorted subject directory: XCOD or XUOD)
/       int    uaf;         (subject names file:       XCON or XUON)
/       int    utd;         (subject directory:        XCSD or XUSD)
/       int    utf;         (subject text file:        XCSF or XUSF)
/       int    usd;         (subject names directory:  XCND or XUND)
/       int    usf;         (subject names file:       XCNN or XUNN)
/       char  *owner;       (psubject owner id)
/       int    n;           (subject number)
/
/    Purpose:
/       Delete an existing SUBJECT, number N.
/
/    How it works:
/       Delete subject N.  Use the first column of unit numbers (shown
/       above) for OSUBJECTS, the second column for PSUBJECTS.
/       PSUBJECTS must also have an OWNER userid.
/
/    Returns: 1 on success
/             0 on failure (bad number N, or database failure)
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  chg_dsubject() only!
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  subj/subdel.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:56 Source code master starting point */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  4/09/91 13:51 Add new confnum argument to updatetext() call. */
/*: CR  7/18/91 14:37 New CNUM arg, new form for calling by chg_dsubject().*/
/*: CR  7/23/91 19:35 Replace updatetext() with updatepart(). */
/*: CR  7/26/91 18:31 Use null.str instead of NULL. */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: CR 11/07/91 13:37 Add confname as argument. */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: DE  3/24/92 12:11 Chixified */
/*: DE  4/20/92 12:59 Cast call params and added arguments */
/*: CX  5/28/92 18:02 Add tbuf arg to updatepart() call. */
/*: JX  5/29/92 16:17 Fix 4th arg to updatepart. */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: JV  5/04/93 10:00 Add attach arg to updatepart() calls. */
/*: CP  5/06/92 12:30 Add L(0) as modnames 'item' arg. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"

extern union  null_t null;
extern Textbuf       tbuf;

FUNCTION  subdelete (cnum, cname, uad,uaf, utd,utf, usd,usf, owner, subject)
   int    cnum, uad,uaf, utd,utf, usd,usf, subject;
   Chix   owner, cname;
{
   struct namlist_t *add, *del, *nlnames(), *nlnode();
   Chix   name, header, subhead, alpha;
   Chix   s0, s1, getline2();
   int    success;

   ENTRY ("subdelete", "");

   name     = chxalloc (L(80), THIN, "subdel name");
   header   = chxalloc (L(60), THIN, "subdel header");
   subhead  = chxalloc (L(20), THIN, "subdel subhead");
   alpha    = chxalloc (L(90), THIN, "subdel alpha");
   success  = 0;

   /*** Get the Header and Name of this subject. */
   s0 = getline2 (utd, utf, cnum, owner, 0, subject, 0, (Textbuf) NULL);
   s1 = getline2 (utd, utf, cnum, owner, 0, subject, 1, (Textbuf) NULL);
   if (s0 == null.chx  ||  s1 == null.chx)  FAIL;
   chxcpy  (header, s0);
   chxcpy  (name,   s1);    chxsimplify (name);
   chxformat (subhead, CQ("%03d"), (int4) subject, L(0), null.chx, null.chx );
  
   /*** First, remove NAME from the alphabetized name-list. */
   chxcpy (alpha, cname);
   chxcat (alpha, CQ(" "));
   chxcat (alpha, name);
   chxcat (alpha, CQ("##"));
   while  (chxalter (alpha, L(0), chxquick(" ",0),chxquick("#",1))) ;
   chxcat (alpha, CQ(" "));
   chxcat (alpha, subhead);
   add = nlnode (1);
   del = nlnode (1);
   nladd (del, alpha, 0);
   modnames (uad, uaf, owner, cnum, L(0), add, del, NOPUNCT);
   nlfree   (del);

   /*** Remove the words in the subject NAME from the subject name files. */
   del = nlnames (name, subhead);
   modnames (usd, usf, owner, cnum, L(0), add, del, NOPUNCT);
   nlfree   (del);
   nlfree   (add);

   /*** Delete the entire entry for SUBJECT in the subject text file. */
   chxclear   (subhead);
   chxformat  (subhead, CQ("#%d ---"), (int4) subject, L(0), null.chx, null.chx);
   unit_lock  (utd, WRITE, L(cnum), L(0), L(0), L(0), owner);
   updatepart (utf, cnum, 0, subject, owner, nullchix, subhead, nullchix,
                     -1, nullchix, (Attachment) NULL, tbuf, 0, nullchix);
   unit_unlk  (utd);
   SUCCEED;

 done:

   chxfree (name);
   chxfree (header);
   chxfree (subhead);
   chxfree (alpha);

   RETURN (success);
}
