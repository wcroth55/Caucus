
/*** MATCHNAMES.   Search namelist file(s) by name for particular entries.
/
/    n = matchnames (ud, uf, cnum, owner, names, and, matches, rempunct);
/
/    Parameters:
/       int       n;         (success?)
/       int       ud, uf;    (namelist file unit numbers)
/       int       cnum;      (conference number, if relevant to UD/UF)
/       int       and;       (logical AND names together?)
/       Chix      owner;     (some ud/uf pairs require an owner id)
/       Namelist  names;     (names to match against)
/       Namelist *matches    (place to put the resulting matches)
/       int       rempunct;  (remove punctuation characters?)
/
/    Purpose:
/       Search a type of namelist file (specified by unit numbers UD and UF,
/       conference number CNUM, and the userid in OWNER) for a set of names.
/       (The names are entries in the namelist NAMES.)  Each name is exactly
/       one word.  A name N in NAMES matches an entry W in the namelist file
/       if N is an initial substring of W.)
/
/    How it works:
/       Matchnames() finds all the instances of the names in NAMES in
/       the namelist file(s).  It creates a new namelist, with all of
/       the matches, and changes MATCHES to point to that namelist.
/
/       Each entry in MATCHES will have a name and a "tag".  If AND
/       is non-zero, matchnames() reduces MATCHES to the set of all
/       entries that have the same "tag"s for *every* name in NAMES.
/
/       Note that MATCHES must be passed in as a pointer to a pointer to
/       a struct namlist_t, as in:
/          struct namlist_t *m;
/          matchnames (..., &m);
/       In this example, M should have *no* value before calling
          matchnames();
/       it is created and allocated by matchnames().  It is up to the caller
/       to free M when it is finished, as in "nlfree (m);".
/
/       Rempunct controls whether matching is done with or without
/       punctuation characters in place.  1 means remove them, 0 means
/       leave them in.
/
/    Returns:
/       0 on error
/       1 if successful
/       2 if successful but ran out of memory -- lists may be incomplete
/       Note that a successful return may still mean an empty MATCHES list.
/
/    Error Conditions:
/       Error if some namelist partfiles cannot be found.
/       Internal errors in nlnode(), nladd(), etc. functions that allocate
/          memory may also cause error return.
/  
/    Side effects:
/
/    Related functions:  matchnl(), loadnmdir()
/
/    Called by:  Practically everybody.
/
/    Operating system dependencies: None.
/
/    Known bugs:
/
/    Home:  people/matchnam.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CR  4/12/90 15:06 Rename strcomp() -> sysstrcomp(). */
/*: CR  8/29/90 13:23 Move code into matchnl(); check with & w/o punct. */
/*: CR  1/28/91 15:54 Words that are all punctuation should not AND in! */
/*: CR  3/14/91 13:40 Update header comments to new template. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  8/02/91 12:05 Add rempunct argument. */
/*: CR 11/07/91 14:07 Add CNUM argument. */
/*: CR 11/21/91 15:57 Add CNUM to matchnl() calls. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: DE  4/13/92 17:02 Chixified */
/*: JX  5/19/92 19:41 Fix Chix. */
/*: CR 12/16/92 14:25 Always "clear" *matches to null.nl. */

#include <stdio.h>
#include "caucus.h"

extern union  null_t         null;

FUNCTION  matchnames (ud, uf, cnum, owner, names, and, matches, rempunct)
   int    ud, uf, and, cnum;
   Chix   owner;
   struct namlist_t *names, **matches;
   int    rempunct;
{
   struct namlist_t *mats, *mres, *mtmp, *a, *b, *n, *namdir;
   struct namlist_t *nlnode();
   short  first;
   Chix   sa, sb, nop;
   int    success;

   ENTRY ("matchnames", "");

   sa       = chxalloc (L(60),  THIN, "matchnames sa");
   sb       = chxalloc (L(60),  THIN, "matchnames sb");
   nop      = chxalloc (L(100), THIN, "matchnames nop");
   success  = 0;
  *matches  = null.nl;

   /*** Get the relevant namepart directory. */
   if (NOT unit_lock (ud, READ, L(cnum), L(0), L(0), L(0), owner))  FAIL;
   namdir = nlnode(4);
   if (NOT loadnmdir (ud, namdir)) { 
      nlfree (namdir);
      unit_unlk (ud);
      FAIL;
   }

   /*** MATCHNAMES takes one word at a time from NAMES, and puts the
   /    list of matches in MRES.  It then ANDs or ORs MRES with the
   /    previous list of results (in MATS), and puts the result back
   /    in MATS.  */
   mats   = nlnode(1);
   mres   = nlnode(1);

   /*** Consider each word in NAMES... */
   first = 1;
   nlclear();
   for (n=names->next;    n!=null.nl && NOT nlerr();    n=n->next) {

      /*** Get the list of matches for STR into MRES. */
      nlfree (mres);
      mres = nlnode(1);
      if (NOT matchnl (mres, n->str, namdir, uf, owner, cnum, first)) 
          continue;

      /*** NOP is STR w/o punctuation chars.  If NOP is empty, do not
      /    AND this match with the others. */
      if (rempunct)  chxpunct (nop, n->str);
      else           chxcpy   (nop, n->str);
      if (EMPTYCHX(nop)  &&  and)  continue;

      /*** Otherwise, if NOP is different from STR, include any matches
      /    it generates in with MRES. */
      if (NOT chxeq (nop, n->str)  &&  NOT EMPTYCHX(nop)) {
         if (NOT matchnl (mres, nop, namdir, uf, owner, cnum, 0))  continue;
      }

      /*** If no matches and AND, then the result is an empty list. */
      if (and  &&  nlsize (mres) == 0) {
         *matches = nlnode(1);
         nlfree (mres);
         nlfree (mats);
         nlfree (namdir);
         unit_unlk (ud);
         SUCCEED;
      }
   

      /*** Remove any entries with duplications of the value string. */
      if (and)  nlrdup (mres, 2);

      /*** If this is the first word, MRES simply becomes MATS. */
      if      (first)  { nlfree (mats);    mats = mres;   mres = nlnode(1); }

      /*** On 2nd or later word and AND, intersect MATS and MRES according
      /    to their value strings, and put the result in MATS. */
      else if (and) {
         mtmp = nlnode(1);
         for (a=mats->next;   a!=null.nl;   a=a->next) {
            chxtoken (sa, null.chx, 2, a->str);
            for (b=mres->next;   b!=null.nl;   b=b->next) {
               chxtoken  (sb, null.chx, 2, b->str);
               if (chxeq (sb, sa)) { nladd (mtmp, a->str, 0);   break; }
            }
         }
         nlfree (mats);
         mats = mtmp;
      }

      /*** On 2nd or later word and OR, MATS = union (MATS, MRES). */
      else {
         for (b=mres->next;   b!=null.nl;   b=b->next)  
            nladd (mats, b->str, 0);
         nlrdup (mats, 2);
      }
      first = 0;
   }

   *matches = mats;
   nlfree (namdir);
   nlfree (mres);
   unit_unlk (ud);
   SUCCEED;

 done:
   
   chxfree ( sa );
   chxfree ( sb );
   chxfree ( nop );

   RETURN ( success );
}
