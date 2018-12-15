/*** A_GET_SUBJ.   Fetch full names of matching subject categories.
/
/    ok = a_get_subj (subj, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Subj      subj;     (object to fill with subject info)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Given the partial name of a subject, fetch the list of
/       full subject names that match.
/
/    How it works:
/       Caller must fill in elements subj->namepart, subj->cnum, and
/       subj->owner (empty for osubjects, userid for psubjects.)
/       MASK is ignored.
/
/    Returns: A_OK on success
/             A_BADARG   bad arguments
/             A_NOCONF   no such conference
/             A_NONE     no such subject
/             A_NOREAD   user not allowed in this conf
/             A_NOTOWNER trying to access other user's psubjects
/             A_DBERR    database error
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/agetsubj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/12/93 08:22 New function. */
/*: CR  2/28/93 22:52 Use cache'd tbuf when possible. */
/*: CP  8/01/93 14:32 Add 'name' arg to make_textbuf() call. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

FUNCTION  a_get_subj (Subj subj, int4 mask, int wait)
{
   Namelist thisname, matches, this, namdir, nlnames(), nlnode();
   Textbuf  tbuf, make_textbuf(), a_cch_tbuf();
   int4     subnum;
   int      uad, uaf, usd, usf, utd, utf;
   int      ok, access, additem, chgresp, chgsubj, success, is_psub, i;
   Chix     cname, temp, userid, cnf, thissubj;
   Chix     pound, blank;
   Chix     line, getline2();

   /*** Check argument validity, clear list of matching subject names. */
   if (subj == (Subj) NULL  ||  subj->tag != T_SUBJ)  return (A_BADARG);
   nlfree (subj->names);
   subj->names = nlnode(0);
   if (subj->cnum <= 0)                               return (A_NOCONF);

   cname    = chxalloc (L(30), THIN, "agetsu cname");
   temp     = chxalloc (L(20), THIN, "agetsu temp");
   userid   = chxalloc (L(20), THIN, "agetsubj userid");
   cnf      = chxalloc (L(30), THIN, "agetsubj cnf");
   thissubj = chxalloc (L(80), THIN, "agetsubj thissubj");

   /*** This user can only work with their own psubjects. */
   is_psub = (NOT EMPTYCHX (subj->owner));
   sysuserid (userid);
   if (is_psub  &&  NOT chxeq (userid, subj->owner))  DONE (A_NOTOWNER);


   /*** Make sure this user can read this conference. */
   if (NOT a_cache (V_GET, subj->cnum, &access, &additem, &chgresp, 
                    &chgsubj, cname))        DONE (A_NOCONF);
   if (access < 1)                           DONE (A_NOREAD);

   /*** Select unit numbers for PSUBJECT or OSUBJECT. */
   if (is_psub) {
      uad = XUOD;   uaf = XUON;          usd = XUND;   usf = XUNN;
      utd = XUSD;   utf = XUSF;
   }
   else {
      uad = XCOD;   uaf = XCON;          usd = XCND;   usf = XCNN;
      utd = XCSD;   utf = XCSF;
   }


   /*** An empty PARTNAME means "select all subjects". */
   if (EMPTYCHX (subj->namepart)) {

      /*** Load the alphabetical subject list namepart directory. */
      if (NOT unit_lock (uad, READ, L(subj->cnum), L(0), L(0), L(0), subj->owner))
                                       DONE (A_DBERR);
      namdir = nlnode(4);
      ok = loadnmdir (uad, namdir);
      unit_unlk (uad);
      if (NOT ok)  { nlfree (namdir);  DONE (A_DBERR); }
      tbuf = make_textbuf (TB_RESPS, TB_LINES, "agetsubj tbuf");

      /*** Load each namepart file in turn, and display contents. */
      for (this=namdir->next;   this != (Namelist) NULL;   this = this->next) {
         if (this->num <= 0)  continue;
         if (NOT unit_lock (uaf, READ, L(subj->cnum), L(this->num), L(0), L(0),
                            subj->owner))                     break;
         if (NOT loadnpart (uaf, this->num, tbuf))
                                          { unit_unlk (uaf);  break; }
         unit_unlk (uaf);

         pound = chxquick ("#", 2);
         blank = chxquick (" ", 3);
         for (i=0;   i<tbuf->l_used;   ++i) {
            chxtoken (temp, nullchix, 1, tbuf->line[i]);
            while (chxalter (temp, L(0), pound, blank)) ;
            chxtoken (cnf,  nullchix, 1, temp);
            if (NOT chxeq (cnf, cname))  continue;
            chxtoken (nullchix, thissubj, 2, temp);
            nladd (subj->names, thissubj, 0);
         }
      }
      nlfree (namdir);
      free_textbuf (tbuf);
   }

   /*** Otherwise, look for the subjects that match PARTNAME. */
   else {
      thisname = nlnames (subj->namepart, nullchix);
      ok = matchnames (usd, usf, subj->cnum, subj->owner, thisname, 
                       AND, &matches, NOPUNCT);
      nlfree (thisname);
      if (NOT ok)                { nlfree (matches);   DONE (A_DBERR);  }
      if (nlsize (matches) == 0) { nlfree (matches);   DONE (A_NONE); }

      /*** For each matching subject, add the full name to NAMES. */
      tbuf = (Textbuf) NULL;
      for (this=matches->next;   this!=(Namelist) NULL;   this=this->next) {

         /*** Find the subject number. */
         chxtoken (temp, nullchix, 2, this->str);
         chxnum   (temp, &subnum);

         /*** See if that subject is in this conference. */
         if (tbuf == (Textbuf) NULL)
            tbuf = a_cch_tbuf (V_GET, utd, utf, subj->cnum, 0, (int) subnum, 
                                      subj->owner);
         line = getline2 (utd, utf, subj->cnum, subj->owner, 0, 
                             (int) subnum, 0, tbuf);
         if (line == nullchix)         continue;
         chxtoken (temp, nullchix, 2, line);
         if (NOT chxeq (temp, cname))  continue;

         line = getline2 (utd, utf, subj->cnum, subj->owner, 0, 
                              (int) subnum, 1, (Textbuf) NULL);
         nladd (subj->names, line, 0);
      }
      nlfree (matches);
   }

   subj->ready = 1;
   DONE (A_OK);

done:
   chxfree (cname);
   chxfree (temp);
   chxfree (userid);
   chxfree (cnf);
   chxfree (thissubj);
   return  (success);
}
