/*** SUBCREATE.  Create a subject category.
/
/    error = subcreate (cnum, cname, uad,uaf, utd,utf, usd,usf, name, owner);
/
/    Parameters:
/       int    error;       (return code)
/       int    cnum;        (conference number)
/       Chix   cname;       (conference name)
/       int    uad;         (sorted subject directory: XCOD or XUOD)
/       int    uaf;         (subject names file:       XCON or XUON)
/       int    utd;         (subject directory:        XCSD or XUSD)
/       int    utf;         (subject text file:        XCSF or XUSF)
/       int    usd;         (subject names directory:  XCND or XUND)
/       int    usf;         (subject names file:       XCNN or XUNN)
/       Chix   name;        (subject name)
/       Chix   owner;       (psubject owner id)
/
/    Purpose:
/       Create a new OSUBJECT or PSUBJECT, with the given NAME.
/
/    How it works:
/       Creates a new subject NAME for conference CNUM.  Use the first
/       column of unit numbers (shown above) for OSUBJECTS, the second
/       column for PSUBJECTS.  PSUBJECTS must also have an OWNER userid.
/
/    Returns: 0       on success
/             DOEXIST if subject NAME already exists.
/             DSUBSET if NAME is a proper subset of an existing subject.
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
/    Home:  subj/subcreat.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:56 Source code master starting point */
/*: JV  3/28/89 14:23 Removed mtu declaration. */
/*: CW  5/19/89 10:40 Add XW unit codes. */
/*: CR  3/29/91 17:31 Use AND/OR code for matchnames() call. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  4/09/91 13:51 Add new cnum argument to updatetext() call. */
/*: CR  7/17/91 17:46 Add CNUM argument, new header, error return values. */
/*: CR  7/23/91 19:24 Replace updatetext() with updatepart(). */
/*: CR  7/26/91 18:31 Use null.str instead of NULL. */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: CR 11/07/91 13:35 Add confname as an argument. */
/*: CR 11/07/91 14:18 Add confnum arg to matchnames() call. */
/*: CR 11/18/91 11:23 Call submatch() with a temporary copy of NAME. */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: CR  7/17/92 15:15 Leave punctuation in ALSUBnnn file. */
/*: DE  3/24/92 12:11 Chixified */
/*: DE  4/20/92 13:22 Fixed mismatched args */
/*: CX  5/28/92 18:02 Add tbuf arg to updatepart() call. */
/*: JX  5/29/92 16:01 Fix 8th arg to updatepart() call, cast 4th arg. */
/*: JX 10/01/92 14:44 Handle DW space as word separator in alpha listing. */
/*: CI 10/07/92 23:08 Chix Integration. */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: CL 12/11/92 13:48 Long Chxvalue N. */
/*: JV  5/04/93 10:00 Add attach arg to updatepart() calls. */
/*: CP  5/06/92 12:01 Add empty 'item' argument to initnmdir(). */
/*: CP  5/06/92 12:30 Add L(0) as modnames 'item' arg. */
/*: CN  5/16/93 13:27 Add 'cnum' arg to initnmdir(). */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"
#include "derr.h"

extern union null_t  null;
extern Textbuf       tbuf;

FUNCTION  subcreate (cnum, cname, uad,uaf, utd,utf, usd,usf, name, owner)
   int    cnum, uad,uaf, utd,utf, usd,usf;
   Chix   cname, name, owner;
{
   struct namlist_t *add, *del, *matches, *this, *nlnames(), *nlnode();
   Chix   subhead, alpha, cnf, temp, tempsubnum, blanks;
   int4   subnum;
   int    success, bl_len, i;
   short  exists;
   Chix   s, bl_tmp, getline2();

   ENTRY ("subcreate", "");

   cnf         = chxalloc (L(20),  THIN, "subcreat cnf");
   temp        = chxalloc (L(160), THIN, "subcreat temp");
   alpha       = chxalloc (L(100), THIN, "subcreat alpha");
   bl_tmp      = chxalloc (L(160), THIN, "subcreat bl_tmp");
   blanks      = chxalloc (L(160), THIN, "subcreat blanks");
   subhead     = chxalloc (L(160), THIN, "subcreat subhead");
   tempsubnum  = chxalloc (L(40),  THIN, "subcreat tempsubnum");
   success     = 0;

   /*** Does this subject NAME already exist?  See if we can find it
   /    in the alphabetic listing of subject names. */
   chxsimplify (name);
   chxcpy   (alpha, name);
   jixreduce (alpha);
   add = nlnames (alpha, null.chx);
   exists = matchnames (usd, usf, cnum, owner, add, AND, &matches, NOPUNCT);
   nlfree (add);

   /*** If the subject index exists, search the MATCHES for an identical
   /    subject name.  If we found one, complain! */
   if (exists) {
      for (this=matches->next;   this!=null.nl;   this = this->next) {
         /*** Get the subject number... */
         chxtoken (subhead, null.chx, 2, this->str);
         chxnum   (subhead, &subnum);

         /*** And fetch the definition of that subject... */
         s = getline2 (utd, utf, cnum, owner, 0, (int) subnum, 0,
                     (Textbuf) NULL);
         if (s == null.chx)  continue;

         /*** Make sure it's in this conference... */
         chxtoken (cnf, null.chx, 2, s);
         if (NOT chxeq (cnf, cname))  continue;

         /*** And see if it's really an exact match! */
         s = getline2 (utd, utf, cnum, owner, 0, (int) subnum, 1,
                      (Textbuf) NULL);
         chxsimplify (s);
         jixreduce   (s);
         if (chxeq (s, alpha)) {
            nlfree (matches);
            DONE(DOEXIST);
         }
      }
      nlfree (matches);
   }

   /*** If the subject index does not exist, then we need to create
   /    the whole data structure. */
   else {
      /*** Create the subject text partfile. */
      unit_lock (utf, WRITE, L(cnum), L(0), L(0), L(0), owner);
      unit_make (utf);
      unit_write(utf, CQ("#0 0\n"));
      unit_close(utf);
      unit_unlk (utf);

      /*** Create the Subject name & alphabetized subject namefiles. */
      initnmdir (usd, usf, cnum, owner, L(0));
      initnmdir (uad, uaf, cnum, owner, L(0));
   }

   /*** If this subject name is a proper subset of another subject name
   /    (or names), complain. */
   chxcpy (temp, name);
   if (submatch (usd,usf, utd,utf, owner, cnum, cname, temp) != 0)
      DONE(DSUBSET);

   /*** Get the current number of subjects and increment it by one. */
   unit_lock (utd, WRITE, L(cnum), L(0), L(0), L(0), owner);
   unit_lock (utf, READ, L(cnum), L(0), L(0), L(0), owner);
   unit_view (utf);

   unit_read (utf, subhead, L(0));
   unit_close(utf);
   unit_unlk (utf);
   chxtoken (tempsubnum, null.chx, 2, subhead );
   chxnum   (tempsubnum, &subnum );
   chxclear  (subhead);
   chxformat (subhead, CQ("#0 %d"), ++subnum, L(0), null.chx, null.chx);

   /*** Rewrite the number of subjects.  Write the "empty" new subject. */
   updatepart (utf, cnum, 0,  0, owner, nullchix, subhead, nullchix, -1,
                 nullchix, (Attachment) NULL, tbuf, 0, nullchix);
   chxclear   (subhead);
   chxformat  (subhead, CQ("#%d %s"), subnum, L(0), cname, null.chx);
   updatepart (utf, cnum, 0, (int) subnum, owner, nullchix, subhead, name, -1,
                 nullchix, (Attachment) NULL, tbuf, 0, nullchix);
   unit_unlk  (utd);

   /*** Add the words in the subject NAME to the subject name-list files. */
   chxclear  (subhead);
   chxformat (subhead, CQ("%03d"), subnum, L(0), null.chx, null.chx);
   del = nlnode  (1);
   add = nlnames (name, subhead);
   modnames (usd, usf, owner, cnum, L(0), add, del, NOPUNCT);
   nlfree   (add);

   /*** Finally, add the NAME itself to the alphabetized name-list. */
   chxcpy (alpha, cname);
   chxcat (alpha, CQ(" "));
   chxcat (alpha, name);
   chxcat (alpha, CQ("##"));

   /* Substitute '#' for all word-separators. */
   chxspecial (SC_GET, SC_BLANK, blanks);
   bl_len = chxlen (blanks);
   for (i=0; i < bl_len; i++)  {
      chxclear  (bl_tmp);
      chxcatval (bl_tmp, WIDE, chxvalue (blanks, L(i)));
      while  (chxalter (alpha, L(0), bl_tmp, chxquick("#",1))) ;
   }

   chxcat (alpha, CQ(" "));
   chxcat (alpha, subhead);
   add = nlnode (1);
   nladd (add, alpha, 0);
   modnames (uad, uaf, owner, cnum, L(0), add, del, PUNCT);
   nlfree   (add);
   nlfree   (del);

   DONE(NOERR);

 done:

   chxfree (cnf);
   chxfree (temp);
   chxfree (alpha);
   chxfree (bl_tmp);
   chxfree (blanks);
   chxfree (subhead);
   chxfree (tempsubnum);

   RETURN (success);
}
