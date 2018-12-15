/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SUBPARSE.  Parse a subject name, add to an Rlist.
/
/    newptr = subparse (oldptr, words, cnum, mine, first);
/
/    Parameters:
/       Rlist  newptr;      (pointer to last accessed node in Rlist)
/       Rlist  oldptr;      (head of Rlist to add items to)
/       Chix   words;       (list of words in subject name)
/       int    cnum;        (conference number)
/       int    mine;        (1=psubject, 0=osubject)
/       int    first;       (# of 1st response to mark, if unspecified item)
/
/    Purpose:
/       Parse a subject name, add it onto an Rlist.
/
/    How it works:
/       WORDS selects a set of subjects.  Adds the items in that (those)
/       subjects onto the Rlist OLDPTR.
/
/    Returns: pointer to last accessed (added to) node in Rlist OLDPTR.
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
/    Known bugs:
/       We should find some way to supply CONFNAME other than via
/       a global.
/
/    Home:  subj/subparse.c
/ */

/*: AA  7/29/88 17:56 Source code master starting point */
/*: CR  3/29/91 17:31 Use AND/OR code for matchnames() call. */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: CR 11/07/91 14:19 Add confnum arg to matchnames() call. */
/*: DE  3/24/92 12:12 Chixified */
/*: DE  4/20/92 12:30 Fixed args on 'getline2' and 'chxnum' */
/*: CR  5/12/93 23:28 Add 'mark' args, make 'flags' short. */
/*: CP  6/20/93 17:51 Now returns Rlist, new arg list. */
/*: CP  6/22/93 16:29 Add cnum argument. */

#include <stdio.h>
#include "caucus.h"
#include "api.h"

extern Chix         confname;
extern union null_t null;

FUNCTION  Rlist subparse (oldptr, words, cnum, mine, first)
   Rlist  oldptr;
   Chix   words;
   int    cnum, mine, first;
{
   struct namlist_t *names, *this, *matches, *nlnames();
   Chix   owner, str, word;
   int4   subnum;
   short  usd, usf, utd, utf;
   short  ok, i;
   Chix   s, getline2();
   Rlist  rl, num_to_rl();

   ENTRY ("subparse", "");

   owner = chxalloc (L(40), THIN, "subparse owner");
   str   = chxalloc (L(80), THIN, "subparse str");
   word  = chxalloc (L(60), THIN, "subparse word");
   rl    = oldptr;

   sysuserid (owner);

   if (mine) {
      usd = XUND;   usf = XUNN;
      utd = XUSD;   utf = XUSF;
   }
   else {
      usd = XCND;   usf = XCNN;
      utd = XCSD;   utf = XCSF;
   }

   /*** Get the list of subjects matching WORDS. */
   names = nlnames (words, null.chx);
   ok = matchnames (usd, usf, cnum, owner, names, AND, &matches, NOPUNCT);
   nlfree (names);
   if (NOT ok)  goto done;

   /*** For each matching subject, find the subject number... */
   for (this=matches->next;   this!=null.nl;   this=this->next) {
      chxtoken (str, null.chx, 2, this->str);
      chxnum   (str, &subnum);

      /*** See if the subject is in this conference. */
      if ( (s = getline2 (utd, utf, cnum, owner, 0, (int) subnum, 0, 
               (Textbuf) NULL)) == null.chx)
         continue;
      chxtoken (word, null.chx, 2, s);
      if (NOT chxeq (word, confname))  continue;

      /*** Add each line of item numbers to the Rlist. */
      for (i=2;
          (s = getline2 (utd, utf, cnum, owner, 0, (int) subnum, i, 
              (Textbuf) NULL)) != null.chx;
           ++i) {
         rl = num_to_rl (rl, s, first);
      }
   }
   nlfree (matches);

done:
   chxfree (owner);
   chxfree (str);
   chxfree (word);

   RETURN  (rl);
}
