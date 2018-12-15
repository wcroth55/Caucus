
/*** ALLCONFS.   Report on new activity in all conferences.
/
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:57 Source code master starting point */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: JV  5/15/91 17:48 Add new loadnpart arg. */
/*: CR  7/27/91 13:33 Use common form of confcheck(). */
/*: CR  8/07/91 14:25 Add 'confnum' arg to syspdir() call. */
/*: JV 11/01/91 15:40 Deal with Local Conf vs True Conf. */
/*: JV 12/09/91 14:00 Only check Local confs. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: JX  7/22/92 17:44 Chixify. */
/*: CI 10/07/92 18:02 Chix Integration. */
/*: CR 10/09/92 18:10 "emptychx" -> EMPTYCHX. */
/*: CR 10/12/92 13:59 Chxfree code. */
/*: JV 10/20/92 10:04 Fixify. */
/*: CL 12/11/92 11:13 Long Chxcatsub args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CP  8/01/93 14:32 Add 'name' arg to make_textbuf() call. */

#include <stdio.h>
#include "caucus.h"

extern struct pdir_t        pdir;
extern union  null_t        null;
extern Chix                 confname;

FUNCTION  allconfs (id)
   Chix   id;
{
   struct namlist_t *namdir, *this, *nlnode();
   Chix   cnum, temp, code;
   int    success;
   int4   num;
   short  i, j, ok;
   char  *strtoken();
   Textbuf tbuf, make_textbuf();

   ENTRY ("allconfs", "");

   cnum = chxalloc (L(10), THIN, "allconfs cnum");
   code = chxalloc (L(10), THIN, "allconfs code");
   temp = chxalloc (L(10), THIN, "allconfs temp");

   tbuf = make_textbuf(TB_RESPS, TB_LINES, "allconfs tbuf");

   /*** Get a list of all conference numbers this user belongs to. */
   syspdir (&pdir, XUPA, 0, 0, id);
   if (pdir.parts == 0)  FAIL;
   success = 0;

   /*** Load the directory of conference names. */
   if (NOT unit_lock (XSCD, READ, L(0), L(0), L(0), L(0), nullchix))  FAIL;
   namdir = nlnode(4);
   ok = loadnmdir (XSCD, namdir);
   unit_unlk (XSCD);
   if (NOT ok)     { nlfree (namdir);            FAIL; }
  
   /*** Load a partfile full of conference names. */
   for (this=namdir->next;   this != null.nl;   this = this->next) {
      if (sysbrktest())    break;
      if (this->num <= 0)  continue;
      if (NOT unit_lock (XSCN, READ, L(0), L(this->num), L(0), L(0),
                         nullchix))  continue;
      if (NOT loadnpart (XSCN, this->num, tbuf))
                                            { unit_unlk (XSCN);  continue; }
      unit_unlk (XSCN);
  
      /*** Check each conference by number: is this user a member? */
      for (i=0;   i < tbuf->l_used;   ++i) {
         if (sysbrktest())    break;
         chxtoken (confname, nullchix, 1, tbuf->line[i]);
         chxtoken (code,     nullchix, 2, tbuf->line[i]);
         if (EMPTYCHX (code) || chxvalue (confname, L(0)) == '!')  continue;

         /*** Only check Local Confnames, but allow old syntax (6=='\0') */
         if (chxvalue (code, L(6)) == 'T')    continue;

         if (chxtoken (cnum, nullchix, 2, tbuf->line[i]) == -1)
          continue;

         chxclear  (temp);
         chxcatsub (temp, cnum, L(2), ALLCHARS);
         chxnum    (temp, &num);

         /*** If so, report on this conference. */
         for (j=0;   j<pdir.parts;   ++j)   if (num == pdir.resp[j]) {
            confcheck (XWTX, (int) num, &success);
            break;
         }
      }
   }
   nlfree (namdir);

done:
   chxfree (cnum);
   chxfree (temp);
   chxfree (code);
   free_textbuf (tbuf);

   RETURN (success);
}
