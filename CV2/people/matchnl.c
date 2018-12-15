
/*** MATCHNL.  Find matches for STR in partfiles UF.  Add to LIST.
/
/    ok = matchnl (list, str, namdir, uf, owner, cnum, first_call);
/
/    Parameters:
/       int       ok;          (success?)
/       Namelist  list;        (add matches to this list)
/       Chix      str;         (name to match)
/       Namelist  namdir;      (namelist of name partfiles)
/       int       uf;          (unit number of name partfiles)
/       Chix      owner;       (owner id of name partfiles)
/       int       cnum;        (conf number for name partfiles)
/       int       first_call   (first call in a sequence?)
/
/    Purpose:
/       Matchnl() is the work-horse function for matchnames().  It
/       does the dirty work of actually finding matches to a string STR.
/
/    How it works:
/       Partfiles to be searched are specified by UF, OWNER, and CNUM.
/       The list of which partfiles begin with which string is 
/       supplied in NAMDIR.
/
/       Matchnl() is often called several times in sequence; FIRST_CALL
/       is 1 if this is the first in such a sequence, else 0.  (It is used
/       to optimize when partfiles must actually be loaded.)
/
/       Matchnl() searches the partfiles for STR, and puts all matches
/       in LIST.
/
/       Because Caucus can create some VERY int4 lists of names against
/       trivial matches, frequently test for <CANCEL> with sysbrktest()
/       and bail out with partial results.
/
/    Returns: 1 on success (a success may have no matches)
/             0 on error loading files.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions: whatnm()
/
/    Called by:  matchnames()
/
/    Operating system dependencies: None.
/
/    Known bugs:
/
/    Home:  people/matchnl.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/29/90 13:03 New function. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: JV  5/15/91 16:52 Add new loadnpart() arg. */
/*: JV  8/13/91 15:00 Add ENTRY. */
/*: CR 11/21/91 15:56 Add CNUM argument. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: JX  5/15/92 16:55 Chixify, PFILEBUF -> Textbuf. */
/*: CR 11/11/92 18:23 Search multiple partfiles if needed. */
/*: CR 11/16/92 16:57 Search multiple partfiles in order from NAMDIR! */
/*: CR 11/18/92 11:55 Bail out of loop when part==0. */
/*: CR 12/11/92 13:06 LEN arg to chxcompare is int4. */
/*: CR  3/17/95 13:14 Bail out on <CANCEL> with partial results. */

#include <stdio.h>
#include "caucus.h"

extern Textbuf tbuf;

extern union null_t null;

FUNCTION  matchnl (list, str, namdir, uf, owner, cnum, first_call)
   struct namlist_t *list, *namdir;
   Chix   str, owner;
   int    uf, first_call, cnum;
{
   Namelist partptr, whatnm();
   static   int loaded;
   int4     len;
   int      part;
   short    lo, md, hi, result;

   ENTRY ("matchnl", "");

   /*** Initialize the (empty) list of matches, and make sure the
   /    partfile that (might) contain STR is already loaded. */
   if (first_call) loaded = -1;
   len   = chxlen (str);

   /*** Loop, scanning multiple names partfiles if necessary, to get
   /    instances of matches to STR. */
   for (partptr = whatnm (str, namdir);   partptr!=null.nl;
        partptr = partptr->next) {

      part = partptr->num;
      if (part == 0)      break;
      if (sysbrktest())   break;

      if (part != loaded) {
         if (NOT unit_lock (uf, READ, L(cnum), L(part), L(0), L(0), owner))  
                                                                 RETURN (0);
         if (NOT loadnpart (uf, part, (Textbuf) NULL))
                                              { unit_unlk (uf);  RETURN (0); }
         unit_unlk (uf);
         loaded  = part;
      }
      if (sysbrktest())   break;
   
   
      /*** Do the binary search until we find a matching initial substring,
      /    or until HI is one record above LO. */
      for (lo=md=0, hi=tbuf->l_used-1, result=1;   hi > lo+1; ) {
         md = (hi + lo) / 2;
   /*
         printf ("hi=%d, lo=%d, md=%d\n", hi, lo, md);
         printf ("ptr[md] = %x\n", text.buf + line.ptr[md]);
         printf ("ptr[md] = '%s'\n", text.buf + line.ptr[md]);
   */
         result = chxcompare (tbuf->line[md], str, len);
         if      (result > 0)  hi = md;
         else if (result < 0)  lo = md;
         else                  break;
      }
   
      /*** Backup over possible matches before MD. */
      while (md > 1  &&  
             chxcompare (tbuf->line[md-1], str, len) == 0) --md;
   
      /*** Now go forward and pick up all matches. */
      while (md < tbuf->l_used-1  &&
             chxcompare (tbuf->line[md], str, len) == 0) {
         if (sysbrktest())  break;
         nladd (list, tbuf->line[md], 0); 
/*       nlcat (list, tbuf->line[md], 0); */
         ++md;
      }
   
      /*** If we ceased finding matches before the end of this partfile,
      /    we're done. */
      if (md < tbuf->l_used-1)  break;
      if (sysbrktest())         break;
   }

   RETURN (1);
}
