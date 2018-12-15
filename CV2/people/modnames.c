/*** MODNAMES.  Add & Delete names to namelist partfile(s).
/
/    n = modnames (ud, uf, owner, cnum, item, add0, del0, rempunct);
/
/    Parameters:
/       int       n;         (success?)
/       int       ud, uf;    (namelist file unit numbers)
/       Chix      owner;     (some ud/uf pairs require an owner id)
/       int       cnum;      (conference number when relevant)
/       int4      item;      (item number when relevant)
/       Namelist  add0;      (names to add)
/       Namelist  del0;      (names to delete)
/       int       rempunct;  (remove punctuation characters?)
/
/    Purpose:
/       Examine the namelist file(s) (specified by unit numbers UD and UF,
/       conference number CNUM, item number ITEM [if relevant] and
/       the userid in OWNER).  Add each entry in namelist ADD0 to the
/       appropriate places & partfiles.  Look for each entry in namelist 
/       DEL0, and delete them from the partfiles.
/
/    How it works:
/       Each entry in ADD0 and DEL0 has both a name and a tag (or value).
/       Entries from ADD0 are placed in the namelist partfiles, retaining
/       the sorted order of the data in the partfiles.  Deletion of entries
/       in the partfiles occur ONLY if the name and tag in the partfile
/       matches EXACTLY the name and tag of an entry in DEL0.
/
/       If REMPUNCT is true, remove the punctuation characters from ADD0
/       and DEL0 before modifying the namelist partfiles.  (Don't actually
/       change the arguments passed by the caller, make copies and change
/       the copies.)
/
/    Returns: 0 on error (can't open needed files), 1 otherwise.
/       (Note that a "successful" returndoes not guarantee that all
/       entries in DEL0 had a match that could be deleted.)
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:  loadnmdir(), stornmdir(), loadnpart(), stornpart().
/
/    Called by:  Practically everybody.
/
/    Operating system dependencies: None.
/
/    Known bugs:
/
/    Home:  people/modnames.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CR 12/20/88 17:45 Raise size of allowed strings. */
/*: CR  4/12/90 15:07 Rename strcomp() -> sysstrcomp(). */
/*: CR  8/29/90 11:50 Use nlpunct() to modify punctuation in add, del lists.*/
/*: CR  8/30/90 11:36 Force remove("a") to happen before add ("a"). */
/*: CR  3/14/91 17:04 Expand header comments with new template. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: JV  5/15/91 16:53 Add new loadnpart arg. */
/*: CR  8/02/91 12:30 Add new argument rempunct. */
/*: CR 11/18/91 17:08 Add CNUM as argument. */
/*: CR 11/21/91 15:07 Add CNUM arg to stornpart() call. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: JX  5/15/92 16:59 Chixify, PFILEBUF->Textbuf. */
/*: CX  5/27/92 14:56 Add tbuf arg to nameinsert(). */
/*: JX  7/06/92 15:02 Use local, static tbuf; don't free tbuf members!. */
/*: CR 10/22/92 11:57 Check length of namdir->str, not chxtype. */
/*: CR 11/16/92 16:53 Whatnm() now returns a Namelist. */
/*: CP  5/06/92 12:19 Add 'item' argument. */
/*: CP  8/01/93 14:32 Add 'name' arg to make_textbuf() call. */

#include <stdio.h>
#include "caucus.h"

extern union  null_t null;

FUNCTION  modnames (ud, uf, owner, cnum, item, add0, del0, rempunct)
   int    ud, uf, cnum;
   Chix   owner;
   int4   item;
   struct namlist_t *add0, *del0;
   int    rempunct;
{
   struct   namlist_t *add1, *add, *del1, *del, *namdir, *nlnode(), *nlpunct();
   Namelist partptr;
   short    remove, loaded, changed, part, lo, hi, md, result, i;
   Chix     str;
   Namelist nlcopy(), whatnm();
   static   Textbuf tbuf = (Textbuf) NULL;
   Textbuf  make_textbuf();
   int      success;

   ENTRY ("modnames", "");

   str     = chxalloc (L(120), THIN, "modnames str");
   if (tbuf == (Textbuf) NULL) 
      tbuf = make_textbuf(TB_RESPS, TB_LINES, "modnames tbuf");
   success = 0;

   /*** Load the relevant namepart directory. */
   if (NOT unit_lock (ud, WRITE, L(cnum), item, L(0), L(0), owner))  FAIL;
   namdir = nlnode(4);
   if (NOT loadnmdir (ud, namdir)) {
      nlfree (namdir);
      unit_unlk (ud);
      FAIL;
   }

   /*** If REMPUNCT is true, modify the ADD list so it has no punctuation;
   /    modify the DEL list to have words both with and without punctuation.*/
   if (rempunct)  { add1 = nlpunct (add0, 0);    del1 = nlpunct (del0, 1); }
   else           { add1 = nlcopy  (add0);       del1 = nlcopy  (del0);    }

   add = add1->next;
   del = del1->next;
   loaded  = -1;          /* Which partfile is currently loaded? */
   changed =  0;          /* Has that partfile been changed?     */

   while (add != null.nl  ||  del != null.nl) {

      /*** Get the alphabetic next string from either ADD or DEL. */
      if      (add == null.nl)                             remove = 1;
      else if (del == null.nl)                             remove = 0;
      else if (chxcompare (add->str, del->str, ALLCHARS) >= 0)       remove = 1;
      else                                                 remove = 0;

      if (remove)  { chxcpy (str, del->str);   del = del->next; }
      else         { chxcpy (str, add->str);   add = add->next; }

      partptr = whatnm (str, namdir);
      part    = (partptr==null.nl ? 0 : partptr->num);
      if (part != loaded) {
         if (changed)  stornpart (ud, uf, owner, cnum, item, namdir, 
                                      loaded, tbuf);
         if (NOT unit_lock (uf, WRITE, L(cnum), L(part), item, L(0), owner))    
                                                               continue;
         if (NOT loadnpart (uf, part, tbuf))
                                          { unit_unlk (uf);    continue; }
         unit_unlk (uf);
         loaded  = part;
         changed = 0;
      }


      /*** Do the binary search until we find a matching initial substring,
      /    or until HI is one record above LO. */
      for (lo=0, hi=tbuf->l_used-1, md=result=1;   hi > lo+1; ) {
         md = (hi + lo) / 2;
         result = chxcompare (tbuf->line[md], str, ALLCHARS);
         if      (result > 0)  hi = md;
         else if (result < 0)  lo = md;
         else                  break;
      }

      /*** If we found a name to be removed, do so and mark partfile
      /    as changed. */
      if (remove  &&  result==0) {
         chxfree (tbuf->line[md]);
         for (i=md+1;   i<tbuf->l_used;   ++i)  
            tbuf->line[i-1] = tbuf->line[i];
         tbuf->line[i-1] = chxalloc (L(20), THIN, "modnames tbuf");
         --tbuf->l_used;
         changed = 1;
      }

      /*** If we have a name to be added, do so and mark the partfile
      /    as changed. */
      if (NOT remove) {
         if (result < 0)  ++md;
         nameinsert (str, md, tbuf);
         changed = 1;
      }
   }

   if (changed)  stornpart (ud, uf, owner, cnum, item, namdir, loaded, tbuf);

   /*** If NAMDIR was changed, rewrite it before unlocking it. */
   if (NOT EMPTYCHX(namdir->str))  stornmdir (ud, namdir);
   nlfree (namdir);
   unit_unlk (ud);
   nlfree (add1);
   nlfree (del1);

   SUCCEED;

 done:

   chxfree (str);

   RETURN (success);
}
